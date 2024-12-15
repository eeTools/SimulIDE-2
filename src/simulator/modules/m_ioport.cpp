/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QDebug>

#include "m_ioport.h"
#include "fblock.h"
#include "fcomponent.h"
#include "iopin.h"

#include "intprop.h"
#include "stringprop.h"

QString mIoPort::m_moduleType = "IoPort";

moduleItem_t mIoPort::registerItem(){
    return {
        "IO Port",
        m_moduleType,
        "",
        "ioport.png",
        [](QString id){ return (Module*)new mIoPort( id ); } };
}

mIoPort::mIoPort( QString name )
       : PortBase( name )
       , m_inputSlot("input" , hookInputInt )
       , m_outSignal("output", hookOutputInt )
{
    m_type = m_moduleType;

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
    m_changed = false;
    for( PinBase* pin : m_pins )
    {
        IoPin* ioPin = (IoPin*)pin;
        /// ioPin->initPin();
    }
}

void mIoPort::runStep() // Update outputs
{
    if( !m_changed ) return;
    m_changed = false;

    m_state = m_inputSlot.intData();

    m_outSignal.changed();
    m_component->voltChanged();
}

PinBase* mIoPort::addPin( QString id )
{
    IoPin* pin = new IoPin( 0, QPoint(0, 0), id, m_component );
    return pin;
}
