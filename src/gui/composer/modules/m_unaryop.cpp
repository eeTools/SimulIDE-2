/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QStringList>
#include <math.h>

#include "m_unaryop.h"
#include "fblock.h"

#include "intprop.h"
#include "stringprop.h"

#define tr(str) simulideTr("UnaryOp",str)

listItem_t UnaryOp::registerItem(){
    return {
        "Unary Operation",
        "Operations",
        "gate.png",
        "UnaryOp",
        [](QString id){ return (CompBase*)new UnaryOp( id ); } };
}

UnaryOp::UnaryOp( QString name )
       : Clocked( name )
       , m_inputSlot("input", hookInputInt )
       , m_outSignal("output", hookOutputInt )
{
    m_outSignal.setIntData( &m_output );

    m_slots.emplace_back( &m_inputSlot );
    m_signals.emplace_back( &m_outSignal );

    m_bits = 0;
    setSize( 2 );

    m_unOpList = QStringList() <<"AND"<<"OR"<<"XOR"<<"NOT"
                               <<"Increase"<<"Decrease"
                               <<"Rotate Right"<<"Rotate Left"
                               <<"Encode"<<"Decode";

    QString trOpList = tr("AND")+","+tr("OR")+","+tr("XOR")+","+tr("NOT")
                  +","+tr("Increase")+","+tr("Decrease")
                  +","+tr("Rotate Right")+","+tr("Rotate Left")
                  +","+tr("Encode")+","+tr("Decode");

    setTypeStr("AND");

    /// TODO: Add Clock

    addPropGroup( { "Main",
    {
        new StrProp<UnaryOp>("gtype", "Type", m_unOpList.join(",")+";"+trOpList
                            , this, &UnaryOp::typeStr, &UnaryOp::setTypeStr, propSlot, "enum" ),

        new IntProp<UnaryOp>("size", "bits", ""
                            , this, &UnaryOp::size, &UnaryOp::setSize, propSlot ),

        new StrProp<UnaryOp>("trigger", "Trigger Type",""
                            , this, &UnaryOp::triggerStr, &UnaryOp::setTriggerStr, propNoCopy, "enum" ),
    },0} );
}
UnaryOp::~UnaryOp(){}

void UnaryOp::initModule()
{
    m_input = 0;
    m_output = 0;
}

void UnaryOp::runStep()
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    m_input = m_inputSlot.intData() & m_mask;

    updateOutput();
}

void UnaryOp::updateOutput()
{
    int out = 0;
    int bits = 0;
    if( m_unOpType < 3 ) for( int i=0; i<m_bits; ++i ) if( m_input & 1<<i ) bits++; // Logic ops

    switch( m_unOpType ) {
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
    m_outSignal.changed();
}

int UnaryOp::dtb()
{
    int i = m_bits;
    for( ; i>=0; --i ) if( m_input & 1<<i ) break ;
    return i;
}

void UnaryOp::setTypeStr( QString type )
{
    if( !m_unOpList.contains( type )) type = "AND";
    int t = m_unOpList.indexOf( type );

    m_unOpStr = type;
    m_unOpType = (unOpType_t)t;
}

void UnaryOp::setSize( int bits )
{
    if     ( bits < 1  ) bits = 1;
    else if( bits > 32 ) bits = 32;

    m_bits = bits;
    m_mask = pow( 2, m_bits )-1;
}

