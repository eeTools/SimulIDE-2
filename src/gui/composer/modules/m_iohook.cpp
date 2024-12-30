/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "m_iohook.h"
#include "hook.h"
//#include "simulator.h"
#include "fcomponent.h"

#include "intprop.h"

listItem_t IoHook::registerItem(){
    return {
        "Hook Port",
        "Ports",
        "hookport.png",
        "IoHook",
        [](QString id){ return (CompBase*)new IoHook( id ); } };
}

IoHook::IoHook( QString name )
      : PortBase( name )
{
}
IoHook::~IoHook(){}

void IoHook::setup()
{
    setPropStr("size", "1");
}

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
