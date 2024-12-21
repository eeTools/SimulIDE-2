/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "modsignal.h"

/// TODO: create moduleChange list at initialization
/// or update modCganged at disconnect()

ModSignal::ModSignal( QString name, hookType_t type )
{
    m_name = name;
    m_type = type;
}
ModSignal::~ModSignal(){}

void ModSignal::connect( ModSlot* slot, bool* modChanged, bool* fCompChanged )
{
    slot->m_signalInt = m_intData;
    slot->m_signalDbl = m_dblData;

    m_fCompChanged = fCompChanged;

    for( bool* mChanged : m_moduleChanged )
        if( mChanged == modChanged ) return;

    m_moduleChanged.emplace_back( modChanged );
}

void ModSignal::changed()
{
    if( !m_fCompChanged ) return;

    *m_fCompChanged = true;

    for( bool* modChanged : m_moduleChanged ) *modChanged = true;
}

//----------------------  SLOT  --------------------------------

ModSlot::ModSlot( QString name, hookType_t type )
{
    m_name = name;
    m_type = type;

    clear();
}
ModSlot::~ModSlot(){;}

void ModSlot::clear()
{
    nextSlot = nullptr;
    m_signalInt = nullptr;
    m_signalDbl = nullptr;
}
