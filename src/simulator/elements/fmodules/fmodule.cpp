/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "fmodule.h"

fModule::fModule()
{
    //m_component = nullptr;
    m_fCompChanged = nullptr;

    m_slotModules = nullptr;
    m_next  = nullptr;
}
fModule::~fModule(){}


void fModule::addSlotModule( fModule* module ) // Prepend slot
{
    fModule* slotModule = m_slotModules;
    while( slotModule )                  // Check if already in the list
    {
        if( slotModule == module ) return;
        slotModule = slotModule->m_next;
    }
    module->m_next = m_slotModules;
    m_slotModules = module;
}

void fModule::outputChanged()
{
    fModule* slotModule = m_slotModules;
    while( slotModule )                 // Notify modules
    {
        slotModule->changed();
        slotModule = slotModule->m_next;
    }
    *m_fCompChanged = true;             // Notify fComponent
}
