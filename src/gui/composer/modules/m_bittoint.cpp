/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QStringList>
#include <math.h>

#include "m_bittoint.h"
#include "fblock.h"

listItem_t BitToInt::registerItem(){
    return {
        "Bit to Int",
        "Converters",
        "gate.png",
        "BitToInt",
        [](int id){ return (CompBase*)new BitToInt( id ); } };
}

BitToInt::BitToInt( int id )
        : Module( id )
        , m_outSignal("output", hookOutputInt )
{
    m_outSignal.setIntData( &m_output );
    m_signals.emplace_back( &m_outSignal );

    m_bits = 0;
    setSize( 2 );

    /// TODO: Add Clock

    /*addPropGroup( { "Main",
    {
        new IntProp("size", "bits", "", this, propSlot ),
    },0} );*/
}
BitToInt::~BitToInt(){}

void BitToInt::initModule()
{
    m_output = 0;
}

void BitToInt::runStep()
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    int out = 0;
    for( int i=0; i<m_bits; ++i )
        if( m_slots.at(i)->intData() ) out |= 1<<i;

    if( m_output == out ) return;
    m_output = out;
    m_outSignal.changed();
}

void BitToInt::setSize( int bits )
{
    if     ( bits < 1  ) bits = 1;
    else if( bits > 32 ) bits = 32;

    if( bits == m_bits ) return;

    if( bits > m_bits ) // Add hooks
    {
        for( int i=m_bits; i<bits; ++i )
        {
            ModSlot* slot = new ModSlot("input"+QString::number(i), hookInputBit );
            m_slots.emplace_back( slot );
        }
    }
    else               // Remove hooks
    {
        for( int i=bits; i<m_bits; ++i ) delete m_slots.at(i);
        m_slots.resize( bits );
    }
    m_bits = bits;

    if( m_funcBlock ) m_funcBlock->updateWidget();
}

