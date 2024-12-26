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

listItem_t IoHook::registerItem(){
    return {
        "IO Hook",
        "Ports",
        "delay.png",
        "IoHook",
        [](QString id){ return (CompBase*)new IoHook( id ); } };
}

IoHook::IoHook( QString name )
      : PortBase( name )
{
    addPropGroup( { "Main",
    {
        new IntProp<IoHook>("dir", "Direction", ""
                           , this, &IoHook::direction, &IoHook::setDirection, 0 ),
    },0} );
}
IoHook::~IoHook(){}

void IoHook::runStep()
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    m_state = m_inputSlot.intData();

    m_outSignal.changed();
    m_component->voltChanged();
}

PinBase* IoHook::addPin( QString id )
{
    Hook* pin = new Hook( 0, QPoint(0, 0), id, hookNone, m_component );
    return pin;
}
