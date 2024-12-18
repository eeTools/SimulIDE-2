/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QStringList>
#include <math.h>

#include "m_abop.h"
#include "fblock.h"

#include "intprop.h"
#include "stringprop.h"

QString AbOp::m_moduleType = "AbOp";

moduleItem_t AbOp::registerItem(){
    return {
        "Binary Operation",
        m_moduleType,
        "",
        "gate.png",
        [](QString id){ return (Module*)new AbOp( id ); } };
}

AbOp::AbOp( QString name )
    : Module( name )
    , m_inSlotA("input A", hookInputInt )
    , m_inSlotB("input B", hookInputInt )
    , m_outSignal("output", hookOutputInt )
{
    m_type = m_moduleType;
    m_abOpType = AND;

    m_outSignal.setIntData( &m_output );

    m_slots.emplace_back( &m_inSlotA );
    m_slots.emplace_back( &m_inSlotB );
    m_signals.emplace_back( &m_outSignal );

    m_bits = 0;
    setSize( 2 );

    m_abOpList = QStringList()<<"AND"<<"OR"<<"XOR"
                              <<"Add"<<"Substract"
                              <<"Multiply"<<"Divide"
                              <<"Modulo"<<"Compare";

    /// TODO: Add Clock

    addPropGroup( { "Main",
    {
        new StrProp<AbOp>("gType", "Type", m_abOpList.join(",")
                         , this, &AbOp::typeStr, &AbOp::setTypeStr,propSlot,"enum" ),

        new IntProp<AbOp>("size", "bits", ""
                         , this, &AbOp::size, &AbOp::setSize, propSlot ),
    },0} );
}
AbOp::~AbOp(){}

void AbOp::initModule()
{
    m_inputA = 0;
    m_inputB = 0;
    m_output = 0;
}

void AbOp::runStep()
{
    if( !m_changed ) return;
    m_changed = false;

    m_inputA = m_inSlotA.intData() & m_mask;
    m_inputB = m_inSlotB.intData() & m_mask;

    updateOutput();
}

void AbOp::updateOutput()
{
    int out;

    switch( m_abOpType ) {
        case AND: out = m_inputA & m_inputB; break;
        case  OR: out = m_inputA | m_inputB; break;
        case XOR: out = m_inputA ^ m_inputB; break;
        case ADD: out = m_inputA + m_inputB; break;
        case SUB: out = m_inputA - m_inputB; break;
        case MUL: out = m_inputA * m_inputB; break;
        case DIV: out = m_inputA / m_inputB; break;
        case MOD: out = m_inputA % m_inputB; break;
        case CMP: out = cmp();               break;
    }

    if( m_output == out ) return;
    m_output = out;
    m_outSignal.changed();
}

int AbOp::cmp()
{
    int o = m_inputA - m_inputB;
    if( o  < 0 ) return 1;
    if( o == 0 ) return 2;
    if( o  > 0 ) return 4;
    return -1;
}

void AbOp::setTypeStr( QString type )
{
    if( !m_abOpList.contains( type )) type = "AND";
    int t = m_abOpList.indexOf( type );

    m_abOpStr = type;
    m_abOpType = (abOpType)t;
}

void AbOp::setSize( int bits )
{
    if     ( bits < 1  ) bits = 1;
    else if( bits > 32 ) bits = 32;

    m_bits = bits;
    m_mask = pow( 2, m_bits )-1;
}

