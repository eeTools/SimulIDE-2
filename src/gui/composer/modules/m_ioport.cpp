/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QDebug>

#include "m_ioport.h"
#include "fcomponent.h"
#include "iopin.h"

#include "uintprop.h"
#include "doubleprop.h"
#include "stringprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("mIoPort",str)

listItem_t mIoPort::registerItem(){
    return {
        "Pin Port",
        "Ports",
        "pinport.png",
        "IoPort",
        [](QString id){ return (CompBase*)new mIoPort( id ); } };
}

mIoPort::mIoPort( QString name )
       : PortBase( name )
       , IoPort( name )
{
}
mIoPort::~mIoPort(){;}

void mIoPort::setup()
{
    setPropStr("size", "2");
}

void mIoPort::initModule()
{
    m_modChanged = false;

    m_pinState = 0;
    if( m_direction == 1 ) setOutState( m_state );
}

void mIoPort::runStep() // Update outputs
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

PinBase* mIoPort::addPin( QString id )
{
    IoPin* pin = new IoPin( 0, QPoint(0, 0), id, m_component );
    pin->setOutHighV( 5 );
    return pin;
}

QList<ComProperty*> mIoPort::inputProps()
{
    QList<ComProperty*> props =
    {
        new StrProp<mIoPort>("Family", tr("Logic Family"), m_families.keys().join(",")
                            , this, &mIoPort::family, &mIoPort::setFamily, 0,"enum" ),

        new DoubProp<mIoPort>("SupplyV", tr("Supply Voltage"), "V"
                             , this, &mIoPort::supplyV, &mIoPort::setSupplyV ),

        //new ComProperty("", " ","","",0),
        new ComProperty( "", tr("Inputs:"),"","",0),

        new DoubProp<mIoPort>("Input_High_V", tr("Low to High Threshold"), "V"
                             , this, &mIoPort::inpHighV, &mIoPort::setInpHighV ),

        new DoubProp<mIoPort>("Input_Low_V", tr("High to Low Threshold"), "V"
                             , this, &mIoPort::inpLowV, &mIoPort::setInpLowV ),

        new DoubProp<mIoPort>("Input_Imped", tr("Input Impedance"), "MΩ"
                             , this, &mIoPort::inputImp, &mIoPort::setInputImp )
    };
    return props;
}

QList<ComProperty*> mIoPort::outputProps()
{
    QList<ComProperty*> props =
    {
        new ComProperty("", tr("Outputs:"),"","",0),

        new DoubProp<mIoPort>("Out_High_V", tr("Output High Voltage"), "V"
                             , this, &mIoPort::outHighV, &mIoPort::setOutHighV ),

        new DoubProp<mIoPort>("Out_Low_V", tr("Output Low Voltage"), "V"
                             , this, &mIoPort::outLowV, &mIoPort::setOutLowV ),

        new DoubProp<mIoPort>("Out_Imped", tr("Output Impedance"), "Ω"
                             , this, &mIoPort::outImp, &mIoPort::setOutImp )
    };
    return props;
}

/*QList<ComProperty*> mIoPort::outputType()
{
    return {
        //new BoolProp<mIoPort>("Inverted", tr("Invert Outputs"), ""
        //                         , this, &mIoPort::invertOuts, &mIoPort::setInvertOuts, propNoCopy ),

        new BoolProp<mIoPort>("Open_Collector", tr("Open Drain"), ""
                                 , this, &mIoPort::openCol, &mIoPort::setOpenCol, propNoCopy )};
}*/

QList<ComProperty*> mIoPort::edgeProps()
{
    return {
        new DoubProp<mIoPort>("pd_n"  , tr("Delay Multiplier"), ""
                             , this, &mIoPort::propSize, &mIoPort::setPropSize ),

        new UintProp<mIoPort>("tpd_ps", tr("Family Delay"), "ns"
                             , this, &mIoPort::propDelay, &mIoPort::setPropDelay ),

        new UintProp<mIoPort>("tr_ps" , tr("Rise Time"), "ns"
                             , this, &mIoPort::riseTime,  &mIoPort::setRiseTime ),

        new UintProp<mIoPort>("tf_ps" , tr("Fall Time"), "ns"
                             , this, &mIoPort::fallTime,  &mIoPort::setFallTime ) };
}

void mIoPort::setInpHighV( double volt )
{
    if( volt > m_supplyV ) volt = m_supplyV;

    if( m_inHighV == volt ) return;
    m_inHighV = volt;

    for( IoPin* pin : m_ioPins ) pin->setInputHighV( volt );

    LogicFamily::setInpHighV( volt );
}

void mIoPort::setInpLowV( double volt )
{
    if( volt < 0 ) volt = 0;
    if( m_inLowV == volt ) return;
    m_inLowV = volt;

    for( IoPin* pin : m_ioPins ) pin->setInputLowV( volt );
    LogicFamily::setInpLowV( volt );
}

void mIoPort::setOutHighV( double volt )
{
    if( volt > m_supplyV ) volt = m_supplyV;

    if( m_ouHighV == volt ) return;
    m_ouHighV = volt;

    for( IoPin* pin : m_ioPins ) pin->setOutHighV( volt );
    LogicFamily::setOutHighV( volt );
}

void mIoPort::setOutLowV( double volt )
{
    if( volt < 0) volt = 0;

    if( m_ouLowV == volt ) return;
    m_ouLowV = volt;

    for( IoPin* pin : m_ioPins ) pin->setOutLowV( volt );
    LogicFamily::setOutLowV( volt );
}

void mIoPort::setInputImp( double imp )
{
    if( imp < 1e-14 ) imp = 1e-14;

    if( m_inImp == imp ) return;
    m_inImp = imp;

    //Simulator::self()->pauseSim();
    for( IoPin* pin : m_ioPins ) pin->setInputImp( imp );
    //Simulator::self()->resumeSim();
}

void mIoPort::setOutImp( double imp )
{
    if( imp < 1e-14 ) imp = 1e-14;

    if( m_ouImp == imp ) return;
    m_ouImp = imp;

    //Simulator::self()->pauseSim();
    for( IoPin* pin : m_ioPins ) pin->setOutputImp( imp );
    //Simulator::self()->resumeSim();
}

/*void mIoPort::setInvertOuts( bool invert )
{
    //if( m_invOutputs == invert ) return;
    m_invOutputs = invert;

    Simulator::self()->pauseSim();
    for( IoPin* pin : m_outPin ) pin->setInverted( invert );
    Circuit::self()->update();
    Simulator::self()->resumeSim();
}

void mIoPort::setInvertInps( bool invert )
{
    //if( m_invInputs == invert ) return;
    m_invInputs = invert;

    Simulator::self()->pauseSim();
    for( IoPin* pin : m_inPin ) pin->setInverted( invert );
    Circuit::self()->update();
    Simulator::self()->resumeSim();
}*/

/*void mIoPort::setOpenCol( bool op )
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

void mIoPort::setRiseTime( uint64_t time )
{
    LogicFamily::setRiseTime( time );
    for( IoPin* pin : m_ioPins ) pin->setRiseTime( m_timeLH*1.25 ); // Time for Output voltage to switch from 10% to 90% (1 gate)
}

void mIoPort::setFallTime( uint64_t time )
{
    LogicFamily::setFallTime( time );
    for( IoPin* pin : m_ioPins ) pin->setFallTime( m_timeHL*1.25 ); // Time for Output voltage to switch from 90% to 10% (1 gate)
}

void mIoPort::setSize(int size )
{
    PortBase::setSize( size );

    pinMode_t pinMode = m_direction ? output : input;

    m_ioPins.clear();
    for( PinBase* pin : m_pins )
    {
        IoPin* ioPin = (IoPin*)pin;
        ioPin->setPinMode( pinMode );
        m_ioPins.emplace_back( ioPin );
    }
    m_numPins = size;
}
