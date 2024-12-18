/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

//#include <QStringList>

#include "m_iohook.h"
#include "hook.h"
#include "simulator.h"
#include "fcomponent.h"

#include "intprop.h"

QString IoHook::m_moduleType = "IoHook";

listItem_t IoHook::registerItem(){
    return {
        "IO Hook",
        "Ports",
        "delay.png",
        m_moduleType,
        [](QString id){ return (CompBase*)new IoHook( id ); } };
}

IoHook::IoHook( QString name )
      : PortBase( name )
      , m_inputSlot("input" , hookInputInt )
      , m_outSignal("output", hookOutputInt )
{
    m_type = m_moduleType;

    m_outSignal.setIntData( &m_outState );

    m_slots.emplace_back( &m_inputSlot );
    m_signals.emplace_back( &m_outSignal );

    addPropGroup( { "Main",
    {
        new IntProp<IoHook>("dir", "Direction", ""
                           , this, &IoHook::direction, &IoHook::setDirection, 0 ),
    },0} );
}
IoHook::~IoHook(){}

void IoHook::runStep()
{
    if( !m_changed ) return;
    m_changed = false;

    m_outState = m_inputSlot.intData();

    m_outSignal.changed();
    m_component->voltChanged();
}

PinBase* IoHook::addPin( QString id )
{
    Hook* pin = new Hook( 0, QPoint(0, 0), id, 0, hookNone, m_component );
    return pin;
}
