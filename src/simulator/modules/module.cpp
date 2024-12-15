/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "module.h"
#include "modsignal.h"

//#include "m_gate.h"
//#include "m_delay.h"
//#include "iopin.h"

Module::Module( QString id )
      : CompBase( id )
{
    //m_name = name;
    m_component = nullptr;
    m_funcBlock = nullptr;
    m_index = 0;
    m_changed = false;
}
Module::~Module(){}

ModSlot* Module::getSlot( QString name )
{
    for( ModSlot* slot : m_slots ) if( slot->name() == name ) return slot;
    return nullptr;
}

void Module::connect( QString signalName, ModSlot* slot, bool* modChanged, bool* fCompChanged )
{
    for( ModSignal* signal : m_signals )
    {
        if( signal->name() != signalName ) continue;

        signal->connect( slot, modChanged, fCompChanged );
        return;
    }
}

