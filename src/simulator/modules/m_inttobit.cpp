/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QStringList>
#include <math.h>

#include "m_inttobit.h"
#include "fblock.h"

#include "intprop.h"

QString IntToBit::m_moduleType = "IntToBit";

moduleItem_t IntToBit::registerItem(){
    return {
        "Int to Bit",
        m_moduleType,
        "",
        "gate.png",
        [](QString id){ return (Module*)new IntToBit( id ); } };
}

IntToBit::IntToBit( QString name )
        : Module( name )
        , m_inputSlot("input", hookInputInt )
{
    m_type = m_moduleType;

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
}

void IntToBit::runStep()
{
    if( !m_changed ) return;
    m_changed = false;

    int input = m_inputSlot.intData() & m_mask;

    if( m_input == input ) return;
    m_input = input;

    int out = 0;
    //for( int i=0; i<m_bits; ++i )

        //m_signals.at(i)->setIntData( m_input & 1<<i );

}

void IntToBit::setSize( int bits )
{
    if     ( bits < 1  ) bits = 1;
    else if( bits > 32 ) bits = 32;

    if( bits == m_bits ) return;

    if( bits > m_bits ) // Add hooks
    {
        for( int i=m_bits; i<bits; ++i )
        {
            ModSignal* signal = new ModSignal("output"+QString::number(i), hookOutputBit );
            m_signals.emplace_back( signal );
        }
    }
    else               // Remove hooks
    {
        for( int i=bits; i<m_bits; ++i ) delete m_signals.at(i);
        m_signals.resize( bits );
    }
    m_bits = bits;
    m_mask = pow( 2, m_bits )-1;

    if( m_funcBlock ){
        m_funcBlock->updateSignals();
        m_funcBlock->updateSize();
    }
}
