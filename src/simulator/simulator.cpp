/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <qtconcurrentrun.h>
#include <QHash>
#include <math.h>

#include "simulator.h"
#include "circuit.h"
#include "updatable.h"
#include "outpaneltext.h"
#include "mainwindow.h"
#include "infowidget.h"
#include "circuitwidget.h"
#include "element.h"
#include "pinbase.h"

#include "callback.h"

Simulator* Simulator::m_pSelf = nullptr;

Simulator::Simulator( QObject* parent )
         : QObject( parent )
         , Kcl()
{
    m_pSelf = this;

    m_state = SIM_STOPPED;
    m_fps = 20;
    m_timerId   = 0;
    m_timerTick_ms = 50;   // 50 ms default
    m_psPerSec  = 1e12;
    m_stepSize  = 1e6;
    m_stepsPS   = 1e6;
    m_reactStep = 1e6;
    m_maxNlstp  = 100000;
    m_slopeSteps = 0;

    m_errors[0] = "";
    //m_errors[1] = "Could not solve Matrix";
    m_errors[2] = "Add Event: NULL free event";
    m_errors[3] = "LAST_SIM_EVENT reached";

    m_warnings[1] = "NonLinear Not Converging";
    m_warnings[2] = "Probably Circuit Error";  // Warning if matrix diagonal element = 0.
    m_warnings[100] = "AVR crashed !!!";

    //resetSim();
    //CircuitWidget::self()->setMsg( " "+tr("Stopped")+" ", 1 );

    m_RefTimer.start();
}
Simulator::~Simulator()
{
    m_CircuitFuture.waitForFinished();
}

void Simulator::timerEvent( QTimerEvent* e )  //update at m_timerTick_ms rate (50 ms, 20 Hz max)
{
    e->accept();

    if( m_state == SIM_WAITING ) return;
    uint64_t currentTime = m_RefTimer.nsecsElapsed();

    if( m_error )
    {
        CircuitWidget::self()->powerCircOff();
        CircuitWidget::self()->setError( m_errors.value( m_error ) );
        return;
    }
    else if( m_warning > 0 )
    {
        int type = (m_warning < 100)? 1:2;
        CircuitWidget::self()->setMsg( m_warnings.value( m_warning), type );
        m_warning = -10;
    }
    else if( m_warning < 0 )
    { if( ++m_warning == 0 ) CircuitWidget::self()->setMsg( " "+tr("Running")+" ", 0 ); }

    if( !m_CircuitFuture.isFinished() ) // Stop remaining parallel thread
    {
        simState_t state = m_state;
        m_state = SIM_WAITING;
        m_running = false;
        m_CircuitFuture.waitForFinished();
        m_state = state;
    }

    for( Updatable* el : m_updateList ) el->updateStep();

    // Calculate Simulation Load
    double timer_ns = m_timerTick_ms*1e6;
    uint64_t simLoop = 0;
    if( m_loopTime > m_refTime ) simLoop = m_loopTime-m_refTime;
    m_simLoad = (m_simLoad+100*simLoop/timer_ns)/2;

    // Get Simulation times
    m_realPsPF = m_simTime-m_tStep;
    m_tStep    = m_simTime;

    if( m_state == SIM_RUNNING ) // Run Circuit in a parallel thread
        m_CircuitFuture = QtConcurrent::run( this, &Simulator::runCircuit );

    if( Circuit::self()->animate() ) // Moved here to be in parallel with runCircuit thread
    {
        if( (currentTime-m_updtTime) >= 2e8 ){ // Animate at 5 FPS
            //Circuit::self()->updateConnectors();
            //for( eNode* node : m_eNodeList) node->updateConnectors();
            m_updtTime = currentTime;
        }
    }

    // Calculate Real Simulation Speed
    m_refTime = m_RefTimer.nsecsElapsed();
    uint64_t deltaRefTime = m_refTime-m_lastRefT;
    if( deltaRefTime >= 1e9 )               // We want steps per 1 Sec = 1e9 ns
    {
        double guiLoad = 100*(double)m_guiTime/(double)deltaRefTime;
        m_guiTime = 0;

        m_realSpeed = (m_tStep-m_lastStep)*10.0/deltaRefTime;
        InfoWidget::self()->setRate( m_realSpeed, m_simLoad, guiLoad );
        m_lastStep = m_tStep;
        m_lastRefT = m_refTime;
    }
    InfoWidget::self()->setCircTime( m_tStep );

    m_guiTime += m_RefTimer.nsecsElapsed()-currentTime; // Time in this function
}

void Simulator::runCircuit()
{
    m_running = true;
    solveCircuit(); // Solve any pending changes
    if( !m_running ) return;

    CallBack* event = m_firstEvent;
    uint64_t endRun = m_simTime + m_psPF;     // Run until time reached
    uint64_t nextTime;

    while( event )                          // Simulator event loop
    {
        if( event->time > endRun ) break;   // All events for this Timer Tick are done

        nextTime = m_simTime;
        while( m_simTime == nextTime )      // Run all event with same timeStamp
        {
            m_simTime = event->time;
            m_firstEvent = event->next;
            event->next = nullptr;          // free Event
            /// event->time = 0;            // Not needed
            event->call();                  // Run event callback
            event = m_firstEvent;
            if( event ) nextTime = event->time;
            else break;
        }
        solveCircuit();
        if( !m_running ) break;
        event = m_firstEvent;                // m_firstEvent can be an event added at solveCircuit()
    }
    if( m_running ) m_simTime = endRun;
    m_running = false;
    m_loopTime = m_RefTimer.nsecsElapsed();
}

void Simulator::solveCircuit()
{
    Kcl::solveSystem();
}

void Simulator::resetSim()
{
    m_state    = SIM_STOPPED;
    m_simLoad  = 0;
    m_guiTime  = 0;
    m_error    = 0;
    m_warning  = 0;
    m_lastStep = 0;
    m_tStep    = 0;
    m_lastRefT = 0;
    m_simTime = 1;
    m_updtTime = 0;
    m_NLstep   = 0;
    m_realPsPF = 1;

    InfoWidget::self()->setCircTime( 0 );
    clearEventList();
}

void Simulator::createNodes()
{
    int node = 0;
    m_pinList.clear();
    QStringList pinNames = Circuit::self()->m_pinMap.keys();
    pinNames.sort();
    for( QString pinName : pinNames )
    {
        if( pinName.startsWith("Node") ) continue;
        if( m_pinList.contains( pinName ) ) continue;

        PinBase* pin = Circuit::self()->m_pinMap.value( pinName );
        if( !pin || !pin->conPin()) continue;
        if( pin->wireFlags() & wireBus ) continue;

        //qDebug() <<"--------------createNode "<<i<<node->itemId();
        pin->registerPinsW( node );
        pin->registerEnode( node );
        node++;
    }
    Kcl::setSize( node );
    qDebug() <<"  Created      "<< node << "\teNodes"<<m_pinList.size()<<"Pins";
}

void Simulator::startSim( bool paused )
{
    resetSim();
    setPsPerSec( m_psPerSec );
    m_debug = paused;
    m_state = SIM_STARTING;

    qDebug() <<"\nStarting Circuit Simulation...\n";

    createNodes();
    Kcl::createCells();

    for( Component* comp : m_componentList ) comp->initialize();
    for( Element*   elem : m_elements      ) elem->stampAdmit();

    //Kcl::preCalculate();
    Kcl::initialize();
    qDebug() <<"  Created      "<< Kcl::getNumGroups() << "\tCircuits";
    for( Element* e : m_elements ) e->stampCurrent();

    //bool ok = Kcl::solveSystem();
    //if( ok ) qDebug() << "\nCircuit Matrix looks good";
    //else     qDebug() << "\nCircuit Matrix ERROR";
    Kcl::solveSystem();

    double sps100 = 100*(double)m_psPerSec/1e12; // Speed %

    qDebug()  << "\nSpeed:" <<         sps100      << "%"
              << "\nSpeed:" << (double)m_psPerSec  << "\tps per Sec"
              << "\nFPS:  " <<         m_fps       << "\tFrames per Sec"
              << "\nFrame:" << (double)m_psPF      << "\tps per Frame"
              << "\nNonLi:" << (double)m_maxNlstp  << "\tMax Iterations"
              << "\nReact:" << (double)m_reactStep << "\tps Reactive step";

    qDebug() << "\n    Simulation Running... \n";

    if( paused ) // We are debugging
    {
        m_oldState = SIM_RUNNING;
        m_state    = SIM_PAUSED;
    }
    else m_state = SIM_RUNNING;

    if( m_timerId != 0 ) this->killTimer( m_timerId );               // Stop Timer
    m_refTime  = m_RefTimer.nsecsElapsed();
    m_loopTime = m_refTime;
    m_timerId = this->startTimer( m_timerTick_ms, Qt::PreciseTimer ); // Init Timer
}

void Simulator::stopSim()
{
    if( m_timerId != 0 ){                   // Stop Timer
        this->killTimer( m_timerId );
        m_timerId = 0;
    }
    m_state = SIM_STOPPED;
    if( !m_CircuitFuture.isFinished() ) m_CircuitFuture.waitForFinished();

    qDebug() << "\n    Simulation Stopped ";

    ///for( eNode* node  : m_eNodeList  )  node->setVolt( 0 );
    ///for( eElement* el : m_elementList ) el->initialize();
    ///for( Updatable* el : m_updateList ) el->updateStep();

    clearEventList();
}

void Simulator::pauseSim() // Only pause simulation, don't update UI
{
    if( m_state <= SIM_PAUSED ) return;
    m_oldState = m_state;
    m_state = SIM_PAUSED;
}

void Simulator::resumeSim()
{
    if( m_state != SIM_PAUSED ) return;
    m_state = m_oldState;
}

void Simulator::setFps( uint64_t fps )
{
    m_fps = fps;
    setPsPerSec( m_psPerSec );
}

void Simulator::setStepsPerSec( uint64_t sps )
{
    if( sps < 1 ) sps = 1;

    m_stepsPS = sps;           // Steps per second
    setPsPerSec( m_stepsPS*m_stepSize );
}

void Simulator::setPsPerSec( uint64_t psPs )
{
    if( psPs < 1 ) psPs = 1;

    m_psPerSec = psPs;    // picosecond/second
    m_psPF = psPs/m_fps;  // picosecond/frame

    uint64_t fps = m_fps;
    if( m_psPF == 0 ) // We must lower fps to get at least 1 ps per frame
    {
        m_psPF = 1;
        fps = psPs;
    }
    m_timerTick_ms = 1000/fps;  // in ms

    InfoWidget::self()->setTargetSpeed( 100*m_psPerSec/1e12 );
}

void Simulator::clearEventList()
{
    m_firstEvent = nullptr;
}
void Simulator::addEvent( uint64_t time, CallBack* cb )
{
    if( m_state < SIM_STARTING ) return;

    time += m_simTime;
    CallBack* last  = nullptr;
    CallBack* event = m_firstEvent;

    while( event ){
        if( time <= event->time ) break; // Insert event here
        last  = event;
        event = event->next;
    }
    cb->time = time;

    if( last ) last->next = cb;
    else       m_firstEvent = cb; // List was empty or insert First

    cb->next = event;
}

/*void Simulator::cancelEvents( eElement* el )
{
    if( el->eventTime == 0 ) return;
    eElement* event = m_firstEvent;
    eElement* last  = nullptr;
    eElement* next  = nullptr;
    el->eventTime = 0;

    while( event ){
        next = event->nextEvent;
        if( el == event )
        {
            if( last ) last->nextEvent = next;
            else       m_firstEvent = next;
            event->nextEvent = nullptr;
        }
        else last = event;
        event = next;
}   }*/

void Simulator::addToUpdateList( Updatable* el )
{ if( !m_updateList.contains(el) ) m_updateList.append(el); }

void Simulator::remFromUpdateList( Updatable* el )
{ m_updateList.removeOne(el); }

void Simulator::addToComponentList( Component* c )
{ if( !m_componentList.contains(c) ) m_componentList.append(c); }

void Simulator::remFromComponentList( Component* c )
{ m_componentList.removeOne(c); }

#include "moc_simulator.cpp"
