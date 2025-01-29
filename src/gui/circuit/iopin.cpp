/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QtMath>
#include <QMenu>

#include "iopin.h"
#include "simulator.h"
#include "circuit.h"

IoPin::IoPin( int angle, const QPoint pos, QString id, Component* parent, bool pullup )
     : Pin( angle, pos, id, parent )
     , m_outSource("out@"+id, &m_node )
     , m_inpSource("inp@"+id, &m_node )
     , m_pulSource("pul@"+id, &m_node )
{
    m_pinType = pinIO;
    m_pinAnim = no_anim;
    m_outState = false;
    m_logicState = false;

    m_inpHiVolt = 2.5;
    m_inpLoVolt = 2.5;
    m_outHiVolt = 5;
    m_outLoVolt = 0;
    m_voltage   = 0;

    m_inputAdmit  = 1e6;
    m_outputAdmit = 40;
    m_pullupAdmit = 1e4;

    m_steps = 0;
    m_timeRis = 3750; // picoseconds
    m_timeFal = 3750;

    if( pullup ) m_pulSource.setAdmitance( m_pullupAdmit );

    animate( Circuit::self()->animate() );
}
IoPin::~IoPin(){}

/*void IoPin::runEvent()
{
    if( m_direction != pinOutput )
    {
        m_step = 0;
        return;
    }
    if( m_step == m_steps )
    {
        m_step = 0;
        IoPin::setOutState( m_nextState );
    }else{
        bool nextState = m_inverted ? !m_nextState : m_nextState;

        if( m_pinMode == openCo )
        {
            double step = nextState ? m_step : m_steps-m_step;
            double delta =  qPow( 1e4*step/m_steps, 2 );
            m_gndAdmit = 1/(m_outputImp+delta);
            updtState();
        }else{
            double delta = m_step;
            if( m_step == 0 ) delta = 1e-5;
            if( nextState ) updtVoltage( m_outLowV+delta*(m_outHighV-m_outLowV)/m_steps ); // L to H
            else            updtVoltage( m_outHighV-delta*(m_outHighV-m_outLowV)/m_steps );// H to L
        }
        int time = nextState ? m_timeRis : m_timeFal;
        Simulator::self()->addEvent( time/m_steps, this );
        m_step++;
    }
}*/

void IoPin::scheduleState( bool state, uint64_t time )
{
    if( time )
    {
        ;///Simulator::self()->addEvent( time, this );
    }
    else IoPin::setOutState( m_nextState );
}

bool IoPin::getLogicState()
{
    double volt = getVoltage();

    bool logicState = m_inverted ? !m_logicState : m_logicState;

    if     ( volt > m_inpHiVolt ) logicState = true;
    else if( volt < m_inpLoVolt ) logicState = false;

    m_logicState = m_inverted ? !logicState : logicState;

    return m_logicState;
}

void IoPin::setInverted( bool inverted )
{
    if( inverted == m_inverted ) return;
    m_inverted = inverted;

    //if( m_pinMode > input ) setOutState( m_outState );
    //else                    updtAdmitance( m_admitance );
    update();
}

void IoPin::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
    QMenu* menu = new QMenu();
    QAction* editAction = menu->addAction( QIcon(":/invert.png"),QObject::tr("Invert Pin"));
    QObject::connect( editAction, &QAction::triggered,
                      [=](){ userInvertPin(); } );

    menu->exec( event->screenPos() );
}

// ---- Script Engine -------------------
#include "angelscript.h"
QStringList IoPin::registerScript( asIScriptEngine* engine )
{
    QStringList memberList;
    engine->RegisterObjectType("IoPin", 0, asOBJ_REF | asOBJ_NOCOUNT );

    /*memberList << "setPinMode( uint mode )";
    engine->RegisterObjectMethod("IoPin", "void setPinMode(uint m)"
                                   , asMETHODPR( IoPin, setPinMode, (uint), void)
                                   , asCALL_THISCALL );*/

    memberList << "getInpState()";
    engine->RegisterObjectMethod("IoPin", "bool getInpState()"
                                   , asMETHODPR( IoPin, getLogicState, (), bool)
                                   , asCALL_THISCALL );

    memberList << "void setOutState( bool state )";
    engine->RegisterObjectMethod("IoPin", "void setOutState(bool s)"
                                   , asMETHODPR( IoPin, setOutState, (bool), void)
                                   , asCALL_THISCALL );

    /*memberList << "setStateZ( bool state )";
    engine->RegisterObjectMethod("IoPin", "void setStateZ( bool z )"
                                   , asMETHODPR( IoPin, setStateZ, (bool), void)
                                   , asCALL_THISCALL );*/

    /*memberList << "setOutStatFast( bool state )";
    engine->RegisterObjectMethod("IoPin", "void setOutStatFast(bool s)"
                                   , asMETHODPR( IoPin, setOutStatFast, (bool), void)
                                   , asCALL_THISCALL );*/

    memberList << "scheduleState( bool state, uint64 time )";
    engine->RegisterObjectMethod("IoPin", "void scheduleState( bool state, uint64 time )"
                                   , asMETHODPR( IoPin, scheduleState, (bool,uint64_t), void)
                                   , asCALL_THISCALL );

    memberList << "getVoltage()";
    engine->RegisterObjectMethod("IoPin", "double getVoltage()"
                                   , asMETHODPR( IoPin, getVoltage, (), double)
                                   , asCALL_THISCALL );

    memberList << "setVoltage( double voltage )";
    engine->RegisterObjectMethod("IoPin", "void setVoltage(double v)"
                                   , asMETHODPR( IoPin, setVoltage, (double), void)
                                   , asCALL_THISCALL );

    /*memberList << "setImpedance( double impedance )";
    engine->RegisterObjectMethod("IoPin", "void setImpedance( double imp )"
                                   , asMETHODPR( IoPin, setImpedance, (double), void)
                                   , asCALL_THISCALL );*/
/*
    memberList << "changeCallBack( eElement@ e, bool call )";
    engine->RegisterObjectMethod("IoPin", "void changeCallBack(eElement@ p, bool s)"
                                   , asMETHODPR( IoPin, changeCallBack, (eElement*, bool), void)
                                   , asCALL_THISCALL );
*/
    return memberList;
}

//--------------------------------------------------------
//--------------------------------------------------------

InputPin::InputPin( int angle, const QPoint pos, QString id, Component* parent, bool pullup )
        : IoPin( angle, pos, id, parent, pullup )
{
    m_inpSource.setAdmitance( m_inputAdmit );
}
InputPin::~InputPin(){}

void InputPin::updateStep()
{
    m_pinAnim = getPinAnim( this );
    update();
}

pinAnim_t InputPin::getPinAnim( IoPin* pin )
{
    return pin->getLogicState()? input_high : input_low;
}

//--------------------------------------------------------
//--------------------------------------------------------

OutputPin::OutputPin( int angle, const QPoint pos, QString id, Component* parent, bool pullup )
         : IoPin( angle, pos, id, parent, pullup )
         , m_slopeEvent( this, &OutputPin::runStep )
{
    m_outSource.setAdmitance( m_outputAdmit );
    m_pulSource.setAdmitance(-1 );
}
OutputPin::~OutputPin(){}

void OutputPin::updateStep()
{
    m_pinAnim = getPinAnim( this );
    update();
}

pinAnim_t OutputPin::getPinAnim( IoPin* pin )
{
    return pin->getLogicState()? out_high : out_low;
}

void OutputPin::runStep()
{
    if( m_step == m_steps )
    {
        m_step = 0;
        IoPin::setOutState( m_nextState );
    }else{
        bool nextState = m_inverted ? !m_nextState : m_nextState;

        double delta = m_step;
        if( m_step == 0 ) delta = 1e-5;
        if( nextState ) setVoltage( m_outLoVolt+delta*(m_outHiVolt-m_outLoVolt)/m_steps ); // L to H
        else            setVoltage( m_outHiVolt-delta*(m_outHiVolt-m_outLoVolt)/m_steps ); // H to L

        int time = nextState ? m_timeRis : m_timeFal;
        Simulator::self()->addEvent( time/m_steps, &m_slopeEvent );
        m_step++;
    }
}


//--------------------------------------------------------
//--------------------------------------------------------

BidirPin::BidirPin( int angle, const QPoint pos, QString id, Component* parent, bool pullup )
        : OutputPin( angle, pos, id, parent, pullup )
{
    m_outSource.setAdmitance( m_outputAdmit );
    m_inpSource.setAdmitance( m_inputAdmit );
}
BidirPin::~BidirPin(){}

void BidirPin::updateStep()
{
    if( m_direction == pinInput ) m_pinAnim = InputPin::getPinAnim( this );
    else                          m_pinAnim = OutputPin::getPinAnim( this );

    update();
}

void BidirPin::setDirection( pinDirection_t d )
{
    if( m_direction == d ) return;
    m_direction = d;

    if( m_direction == pinInput )
    {
        m_outSource.setAdmitance( 0 );
        m_inpSource.setAdmitance( m_inputAdmit );
        m_nextState = m_outState;
    }else{
        m_inpSource.setAdmitance( 0 );
        m_outSource.setAdmitance( m_outputAdmit );
        OutputPin::setOutState( m_nextState );
    }
}

//--------------------------------------------------------
//--------------------------------------------------------

OpenColPin::OpenColPin( int angle, const QPoint pos, QString id, Component* parent, bool pullup )
          : IoPin( angle, pos, id, parent, pullup )
          , m_slopeEvent( this, &OpenColPin::runStep )
{
    m_outSource.setAdmitance( m_outputAdmit );
}
OpenColPin::~OpenColPin(){}

void OpenColPin::updateStep()
{
    bool state = getLogicState();

    pinAnim_t lowAnim = m_outState ? driven_low : open_low;
    m_pinAnim = state? open_high : lowAnim ;
    update();
}

void OpenColPin::runStep()
{
    if( m_step == m_steps )
    {
        m_step = 0;
        IoPin::setOutState( m_nextState );
    }else{
        bool nextState = m_inverted ? !m_nextState : m_nextState;

        double step = nextState ? m_step : m_steps-m_step;
        double delta = qPow( 1e4*step/m_steps, 2 );
        m_outSource.updtAdmitance( m_outputAdmit+1/delta );

        int time = nextState ? m_timeRis : m_timeFal;
        Simulator::self()->addEvent( time/m_steps, &m_slopeEvent );
        m_step++;
    }
}

void OpenColPin::setOutState( bool state ) // Set Output to Hight or Low
{
    if( m_steps )
    {
        if( m_nextState == state ) return;
        if( m_step ) m_step = m_steps-m_step;

        m_nextState = state;
        runStep();
        return;
    }
    if( m_outState == state ) return;
    m_outState = state;
    if( m_inverted ) state = !state;

    double admit = state ? 0 : m_outputAdmit;
    m_outSource.updtAdmitance( admit );
}
