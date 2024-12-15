/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "modsignal.h"

/// TODO: create moduleChange list at initialization
/// or update modCganged at disconnect()

ModSignal::ModSignal( QString name, hookType_t type/*, bool* changed*/ )
{
    m_name = name;
    m_type = type;
    //m_changed = changed;
    /// m_slot = nullptr;
}
ModSignal::~ModSignal(){}

void ModSignal::connect( ModSlot* slot, bool* modChanged, bool* fCompChanged )
{
    slot->m_signalInt = m_intData;
    slot->m_signalDbl = m_dblData;
    //slot->m_signalChg = &m_changed;
    /// slot->nextSlot = m_slot; // New slots are prepended (LIFO)
    /// m_slot = slot;

    m_fCompChanged = fCompChanged;

    for( bool* mChanged : m_moduleChanged )
        if( mChanged == modChanged ) return;

    m_moduleChanged.emplace_back( modChanged );
}

/*void ModSignal::disconnect( ModSlot* slot )
{
    ModSlot* preSlot = nullptr;
    ModSlot* posSlot = m_slot;
    while( posSlot )
    {
        if( posSlot == slot )
        {
            if( preSlot ) preSlot->nextSlot = posSlot->nextSlot;
            else          m_slot = posSlot->nextSlot;

            slot->m_signalInt = nullptr;
            slot->m_signalDbl = nullptr;
            slot->nextSlot = nullptr;
            break;
        }
        preSlot = posSlot;
        posSlot = posSlot->nextSlot;
    }
    /// TODO: update modchanged
}*/

void ModSignal::changed()
{
    *m_fCompChanged = true;

    for( bool* modChanged : m_moduleChanged ) *modChanged = true;
}

/*void ModSignal::send( double val ) // Calls all connected slots
{
    ModSlot* slot = m_slot;
    while( slot ){
        slot->setValue( val );
        slot = slot->nextSlot;
    }
}

void ModSignal::send( int val ) // Calls all connected slots
{
    ModSlot* slot = m_slot;
    while( slot ){
        slot->setValue( val );
        slot = slot->nextSlot;
    }
}*/


//----------------------  SLOT  --------------------------------

ModSlot::ModSlot( QString name, hookType_t type/*, bool* changed*/ )
{
    m_name = name;
    m_type = type;

    //m_callBack = nullptr;

    clear();
}
ModSlot::~ModSlot(){;}

void ModSlot::clear()
{
    nextSlot = nullptr;
    //m_signalChg = nullptr;
    m_signalInt = nullptr;
    m_signalDbl = nullptr;
}
