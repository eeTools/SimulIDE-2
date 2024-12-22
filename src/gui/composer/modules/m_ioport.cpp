/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QDebug>

#include "m_ioport.h"
//#include "fblock.h"
#include "fcomponent.h"
#include "iopin.h"

#include "intprop.h"
#include "stringprop.h"

listItem_t mIoPort::registerItem(){
    return {
        "IO Port",
        "Ports",
        "ioport.png",
        "IoPort",
        [](QString id){ return (CompBase*)new mIoPort( id ); } };
}

mIoPort::mIoPort( QString name )
       : PortBase( name )
       , IoPort( name )
       , m_inputSlot("input" , hookInputInt )
       , m_outSignal("output", hookOutputInt )
{
    m_outSignal.setIntData( &m_state );
    m_slots.emplace_back( &m_inputSlot );
    m_signals.emplace_back( &m_outSignal );

    m_sideList = QStringList()<<"Right"<<"Left"<<"Top"<<"Bottom";

    addPropGroup( { "Main",
    {
        new StrProp<mIoPort>("side", "Side", m_sideList.join(",")
                          , this, &mIoPort::side, &mIoPort::setSide,0,"enum" ),

        new IntProp<mIoPort>("pos", "Offset", ""
                           , this, &mIoPort::position, &mIoPort::setPosition, 0 ),

        new StrProp<mIoPort>("pinnames", "Pin Names", ""
                          , this, &mIoPort::pinNames, &mIoPort::setPinNames,0  ),

        new IntProp<mIoPort>("size", "Pins", ""
                           , this, &mIoPort::size, &mIoPort::setSize, propSlot ),

        new IntProp<mIoPort>("dir", "Direction", ""
                           , this, &mIoPort::direction, &mIoPort::setDirection, /*propHidden |*/ propSlot ),
    },0} );
}
mIoPort::~mIoPort(){;}

void mIoPort::setup()
{
    setPropStr("size", "2");
}

void mIoPort::initModule()
{
    m_modChanged = false;

    pinMode_t pinMode = m_direction ? output : input;

    m_ioPins.clear();
    for( PinBase* pin : m_pins )
    {
        IoPin* ioPin = (IoPin*)pin;
        ioPin->setPinMode( pinMode );
        m_ioPins.emplace_back( ioPin );
    }
    m_numPins = m_pins.size();
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
        /// m_component->voltChanged();
    }
}

PinBase* mIoPort::addPin( QString id )
{
    IoPin* pin = new IoPin( 0, QPoint(0, 0), id, m_component );
    pin->setOutHighV( 5 );
    return pin;
}
