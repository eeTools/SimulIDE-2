/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QDebug>

#include "m_pinport.h"
#include "fcomponent.h"
#include "iopin.h"

#include "uintprop.h"
#include "doubleprop.h"
#include "stringprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("mPinPort",str)

listItem_t mPinPort::registerItem(){
    return {
        "Pin Port",
        "Ports",
        "pinport.png",
        "IoPort",
        [](int id){ return (CompBase*)new mPinPort( id ); } };
}

mPinPort::mPinPort( int id )
        : PortModule( id )
        , IoPort( "" )
{
    m_portType = portIO;
}
mPinPort::~mPinPort(){;}

void mPinPort::setup()
{
    setPropStr("size", "2");
}

void mPinPort::initModule()
{
    m_modChanged = false;

    m_pinState = 0;
    if( m_direction == 1 ) setOutState( m_state );
}

void mPinPort::runStep() // Update outputs
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    if( m_direction == 1 ) // Output
    {
        m_state = m_inputSlot.intData();
        setOutState( m_state );
    }
    else                   // Input
    {
        m_state = getInpState();
        m_outSignal.changed();
    }
}

PinBase* mPinPort::addPin( QString id )
{
    IoPin* pin = new IoPin( 0, QPoint(0, 0), id, m_component );
    pin->setOutHighV( 5 );
    return pin;
}

QList<ComProperty*> mPinPort::inputProps()
{
    QList<ComProperty*> props =
    {
        new StrProp<mPinPort>("Family", tr("Logic Family"), m_families.keys().join(",")
                            , this, &mPinPort::family, &mPinPort::setFamily, 0,"enum" ),

        new DoubProp<mPinPort>("SupplyV", tr("Supply Voltage"), "V"
                             , this, &mPinPort::supplyV, &mPinPort::setSupplyV ),

        //new ComProperty("", " ","","",0),
        new ComProperty( "", tr("Inputs:"),"","",0),

        new DoubProp<mPinPort>("Input_High_V", tr("Low to High Threshold"), "V"
                             , this, &mPinPort::inpHighV, &mPinPort::setInpHighV ),

        new DoubProp<mPinPort>("Input_Low_V", tr("High to Low Threshold"), "V"
                             , this, &mPinPort::inpLowV, &mPinPort::setInpLowV ),

        new DoubProp<mPinPort>("Input_Imped", tr("Input Impedance"), "MΩ"
                             , this, &mPinPort::inputImp, &mPinPort::setInputImp )
    };
    return props;
}

QList<ComProperty*> mPinPort::outputProps()
{
    QList<ComProperty*> props =
    {
        new ComProperty("", tr("Outputs:"),"","",0),

        new DoubProp<mPinPort>("Out_High_V", tr("Output High Voltage"), "V"
                             , this, &mPinPort::outHighV, &mPinPort::setOutHighV ),

        new DoubProp<mPinPort>("Out_Low_V", tr("Output Low Voltage"), "V"
                             , this, &mPinPort::outLowV, &mPinPort::setOutLowV ),

        new DoubProp<mPinPort>("Out_Imped", tr("Output Impedance"), "Ω"
                             , this, &mPinPort::outImp, &mPinPort::setOutImp )
    };
    return props;
}

/*QList<ComProperty*> mPinPort::outputType()
{
    return {
        //new BoolProp<mPinPort>("Inverted", tr("Invert Outputs"), ""
        //                         , this, &mPinPort::invertOuts, &mPinPort::setInvertOuts, propNoCopy ),

        new BoolProp<mPinPort>("Open_Collector", tr("Open Drain"), ""
                                 , this, &mPinPort::openCol, &mPinPort::setOpenCol, propNoCopy )};
}*/

QList<ComProperty*> mPinPort::edgeProps()
{
    return {
        new DoubProp<mPinPort>("pd_n"  , tr("Delay Multiplier"), ""
                             , this, &mPinPort::propSize, &mPinPort::setPropSize ),

        new UintProp<mPinPort>("tpd_ps", tr("Family Delay"), "ns"
                             , this, &mPinPort::propDelay, &mPinPort::setPropDelay ),

        new UintProp<mPinPort>("tr_ps" , tr("Rise Time"), "ns"
                             , this, &mPinPort::riseTime,  &mPinPort::setRiseTime ),

        new UintProp<mPinPort>("tf_ps" , tr("Fall Time"), "ns"
                             , this, &mPinPort::fallTime,  &mPinPort::setFallTime ) };
}

void mPinPort::setInpHighV( double volt )
{
    if( volt > m_supplyV ) volt = m_supplyV;

    if( m_inHighV == volt ) return;
    m_inHighV = volt;

    for( IoPin* pin : m_ioPins ) pin->setInputHighV( volt );

    LogicFamily::setInpHighV( volt );
}

void mPinPort::setInpLowV( double volt )
{
    if( volt < 0 ) volt = 0;
    if( m_inLowV == volt ) return;
    m_inLowV = volt;

    for( IoPin* pin : m_ioPins ) pin->setInputLowV( volt );
    LogicFamily::setInpLowV( volt );
}

void mPinPort::setOutHighV( double volt )
{
    if( volt > m_supplyV ) volt = m_supplyV;

    if( m_ouHighV == volt ) return;
    m_ouHighV = volt;

    for( IoPin* pin : m_ioPins ) pin->setOutHighV( volt );
    LogicFamily::setOutHighV( volt );
}

void mPinPort::setOutLowV( double volt )
{
    if( volt < 0) volt = 0;

    if( m_ouLowV == volt ) return;
    m_ouLowV = volt;

    for( IoPin* pin : m_ioPins ) pin->setOutLowV( volt );
    LogicFamily::setOutLowV( volt );
}

void mPinPort::setInputImp( double imp )
{
    if( imp < 1e-14 ) imp = 1e-14;

    if( m_inImp == imp ) return;
    m_inImp = imp;

    //Simulator::self()->pauseSim();
    for( IoPin* pin : m_ioPins ) pin->setInputImp( imp );
    //Simulator::self()->resumeSim();
}

void mPinPort::setOutImp( double imp )
{
    if( imp < 1e-14 ) imp = 1e-14;

    if( m_ouImp == imp ) return;
    m_ouImp = imp;

    //Simulator::self()->pauseSim();
    for( IoPin* pin : m_ioPins ) pin->setOutputImp( imp );
    //Simulator::self()->resumeSim();
}

/*void mPinPort::setInvertOuts( bool invert )
{
    //if( m_invOutputs == invert ) return;
    m_invOutputs = invert;

    Simulator::self()->pauseSim();
    for( IoPin* pin : m_outPin ) pin->setInverted( invert );
    Circuit::self()->update();
    Simulator::self()->resumeSim();
}

void mPinPort::setInvertInps( bool invert )
{
    //if( m_invInputs == invert ) return;
    m_invInputs = invert;

    Simulator::self()->pauseSim();
    for( IoPin* pin : m_inPin ) pin->setInverted( invert );
    Circuit::self()->update();
    Simulator::self()->resumeSim();
}*/

/*void mPinPort::setOpenCol( bool op )
{
    if( m_openCol == op ) return;
    m_openCol = op;
    Simulator::self()->pauseSim();
    for( IoPin* pin : m_outPin )
    {
        if( op ) pin->setPinMode( openCo );
        else     pin->setPinMode( output );
    }
    Circuit::self()->update();
    Simulator::self()->resumeSim();
}*/

void mPinPort::setRiseTime( uint64_t time )
{
    LogicFamily::setRiseTime( time );
    for( IoPin* pin : m_ioPins ) pin->setRiseTime( m_timeLH*1.25 ); // Time for Output voltage to switch from 10% to 90% (1 gate)
}

void mPinPort::setFallTime( uint64_t time )
{
    LogicFamily::setFallTime( time );
    for( IoPin* pin : m_ioPins ) pin->setFallTime( m_timeHL*1.25 ); // Time for Output voltage to switch from 90% to 10% (1 gate)
}

void mPinPort::setSize(int size )
{
    PortModule::setSize( size );

    m_ioPins.clear();
    for( PinBase* pin : m_pins )
    {
        IoPin* ioPin = (IoPin*)pin;
        ioPin->setDirection( (pinDirection_t)m_direction );
        m_ioPins << ioPin;
    }
    m_numPins = size;
    m_component->updatePins();
}
