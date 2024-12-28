/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QStringList>
#include <math.h>

#include "m_inttobit.h"
#include "fblock.h"

#include "intprop.h"

listItem_t IntToBit::registerItem(){
    return {
        "Int to Bit",
        "Converters",
        "gate.png",
        "IntToBit",
        [](QString id){ return (CompBase*)new IntToBit( id ); } };
}

IntToBit::IntToBit( QString name )
        : Module( name )
        , m_inputSlot("input", hookInputInt )
{
    m_slots.emplace_back( &m_inputSlot );

    m_bits = 0;
    setSize( 2 );

    /// TODO: Add Clock

    addPropGroup( { "Main",
    {
        new IntProp<IntToBit>("size", "bits", ""
                             , this, &IntToBit::size, &IntToBit::setSize, propSlot ),
    },0} );
}
IntToBit::~IntToBit(){}

void IntToBit::initModule()
{
    m_input = 0;
    for( int i=0; i<m_bits; ++i ) m_outputs[i] = 0;
}

void IntToBit::runStep()
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    int input = m_inputSlot.intData() & m_mask;

    if( m_input == input ) return;
    m_input = input;

    for( int i=0; i<m_bits; ++i )
    {
        int out = m_input & 1<<i;
        if( out == m_outputs[i] ) continue;
        m_outputs[i] = out;
        m_signals.at(i)->changed();
    }
}

void IntToBit::setSize( int bits )
{
    if     ( bits < 1  ) bits = 1;
    else if( bits > 32 ) bits = 32;

    if( bits == m_bits ) return;

     m_outputs.resize( bits, 0 );

    if( bits > m_bits ) // Add hooks
    {
        for( int i=m_bits; i<bits; ++i )
        {
            ModSignal* signal = new ModSignal("output"+QString::number(i), hookOutputBit );
            m_signals.emplace_back( signal );
            signal->setIntData( &m_outputs.at(i) );
        }
    }
    else               // Remove hooks
    {
        for( int i=bits; i<m_bits; ++i ) delete m_signals.at(i);
        m_signals.resize( bits );
    }
    m_bits = bits;
    m_mask = pow( 2, m_bits )-1;

    if( m_funcBlock ) m_funcBlock->updateWidget();
}

