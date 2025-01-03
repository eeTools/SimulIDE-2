/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "m_hookport.h"
#include "hook.h"
//#include "simulator.h"
#include "fcomponent.h"

#include "intprop.h"

listItem_t mHookPort::registerItem(){
    return {
        "Hook Port",
        "Ports",
        "hookport.png",
        "mHookPort",
        [](QString id){ return (CompBase*)new mHookPort( id ); } };
}

mHookPort::mHookPort( QString name )
         : PortModule( name )
{
    m_portType = portHook;
}
mHookPort::~mHookPort(){}

void mHookPort::setup()
{
    setPropStr("size", "1");
}

void mHookPort::runStep()
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    m_state = m_inputSlot.intData();

    m_outSignal.changed();
    m_component->voltChanged();
}

PinBase* mHookPort::addPin( QString id )
{
    Hook* pin = new Hook( 0, QPoint(0, 0), id, hookNone, m_component );
    return pin;
}
