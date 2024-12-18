/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QStringList>
#include <math.h>

#include "m_bitop.h"
#include "fblock.h"

#include "intprop.h"
#include "stringprop.h"

QString BitOp::m_moduleType = "BitOp";

listItem_t BitOp::registerItem(){
    return {
        "Unary Operation",
        "Operations",
        "gate.png",
        m_moduleType,
        [](QString id){ return (CompBase*)new BitOp( id ); } };
}

BitOp::BitOp( QString name )
     : Clocked( name )
     , m_inputSlot("input", hookInputInt/*, &m_changed*/ )
     , m_outSignal("output", hookOutputInt )
{
    m_type = m_moduleType;
    m_bitOpType = AND;

    m_outSignal.setIntData( &m_output );

    m_slots.emplace_back( &m_inputSlot );
    m_signals.emplace_back( &m_outSignal );

    m_bits = 0;
    setSize( 2 );

    m_bitOpList = QStringList()<<"AND"<<"OR"<<"XOR"<<"NOT"
                               <<"Increase"<<"Decrease"
                               <<"Rotate Right"<<"Rotate Left"
                               <<"Encode"<<"Decode";

    /// TODO: Add Clock

    addPropGroup( { "Main",
    {
        new StrProp<BitOp>("gType", "Type", m_bitOpList.join(",")
                         , this, &BitOp::typeStr, &BitOp::setTypeStr, propSlot, "enum" ),

        new IntProp<BitOp>("size", "bits", ""
                         , this, &BitOp::size, &BitOp::setSize, propSlot ),

        new StrProp <BitOp>("Trigger", "Trigger Type",""
                         , this, &BitOp::triggerStr, &BitOp::setTriggerStr, propNoCopy, "enum" ),
    },0} );
}
BitOp::~BitOp(){}

void BitOp::initModule()
{
    m_input = 0;
    m_output = 0;
}

void BitOp::runStep()
{
    if( !m_changed ) return;
    m_changed = false;

    m_input = m_inputSlot.intData() & m_mask;
    //for( int i=0; i<m_bits; ++i )
    //    if( m_bitSlots.at(i)->intData() ) m_input |= 1<<i;

    updateOutput();
}

/*void BitOp::setState( int s )
{
    m_input = s;
    updateOutput();
}*/

/*void BitOp::setBitState( int s, int bit )
{
    if( s ) m_input |= 1<<bit;
    else    m_input &= ~(1<<bit);
    updateOutput();
}*/

void BitOp::updateOutput()
{
    // Calculate output

    int out = 0;
    int bits = 0;
    if( m_bitOpType < 3 ) for( int i=0; i<m_bits; ++i ) if( m_input & 1<<i ) bits++; // Logic ops

    switch( m_bitOpType ) {
        case AND: out = bits == m_bits; break;
        case  OR: out = bits  > 0;      break;
        case XOR: out = bits == 1;      break;
        case NOT: out = !m_input;       break;
        case INC: out = m_input+1;      break;
        case DEC: out = m_input-1;      break;
        case RTL: out = m_input<<1;     break;
        case RTR: out = m_input>>1;     break;
        case DTB: out = dtb();          break;
        case BTD: out = 1<<m_input;     break;
    }

    if( m_output == out ) return;
    m_output = out;
    //m_outSignal.send( out );
    m_outSignal.changed();
}

int BitOp::dtb()
{
    int i = m_bits;
    for( ; i>=0; --i ) if( m_input & 1<<i ) break ;
    return i;
}

void BitOp::setTypeStr( QString type )
{
    if( !m_bitOpList.contains( type )) type = "AND";
    int t = m_bitOpList.indexOf( type );

    m_bitOpStr = type;
    m_bitOpType = (bitOpType)t;
    ///if( m_bitOpType == NOT ) setSize( 1 );
}

void BitOp::setSize( int bits )
{
    if     ( bits < 1  ) bits = 1;
    else if( bits > 32 ) bits = 32;

    m_bits = bits;
    m_mask = pow( 2, m_bits )-1;

    //if( bits == m_bits ) return;

    /*if( bits > m_bits ) // Add hooks
    {
        for( int i=m_bits; i<bits; ++i )
        {
            ModSlot* slot = new ModSlot("input"+QString::number(i), hookInputInt );
            m_slots.emplace_back( slot );
            m_bitSlots.emplace_back( slot );
        }
    }
    else               // Remove hooks
    {
        for( int i=bits; i<m_bits; ++i ) delete m_slots.at(i+1);
        m_slots.resize( bits+1 );
        m_bitSlots.resize( bits );
    }*/
    //m_bits = bits;

    /*if( m_funcBlock ){
        m_funcBlock->updateSlots();
        m_funcBlock->updateSize();
    }*/
}

