/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QStringList>
#include <math.h>

#include "m_binaryop.h"
#include "fblock.h"

#include "intprop.h"
#include "stringprop.h"

#define tr(str) simulideTr("BinaryOpOp",str)

listItem_t BinaryOp::registerItem(){
    return {
        "Binary Operation",
        "Operations",
        "gate.png",
        "BinaryOp",
        [](QString id){ return (CompBase*)new BinaryOp( id ); } };
}

BinaryOp::BinaryOp( QString name )
        : Module( name )
        , m_inSlotA("inputA", hookInputInt )
        , m_inSlotB("inputB", hookInputInt )
        , m_outSignal("output", hookOutputInt )
{
    m_binOpType = AND;

    m_outSignal.setIntData( &m_output );

    m_slots.emplace_back( &m_inSlotA );
    m_slots.emplace_back( &m_inSlotB );
    m_signals.emplace_back( &m_outSignal );

    m_bits = 0;
    setSize( 2 );

    m_binOpList = QStringList() <<"AND"<<"OR"<<"XOR"
                                <<"Add"<<"Substract"
                                <<"Multiply"<<"Divide"
                                <<"Modulo"<<"Compare";

    QString trOpList = tr("AND")+","+tr("OR")+","+tr("XOR")
                  +","+tr("Add")+","+tr("Substract")
                  +","+tr("Multiply")+","+tr("Divide")
                  +","+tr("Modulo")+","+tr("Compare");

    /// TODO: Add Clock

    addPropGroup( { "Main",
    {
        new StrProp<BinaryOp>("gType", "Type", m_binOpList.join(",")+";"+trOpList
                         , this, &BinaryOp::typeStr, &BinaryOp::setTypeStr,propSlot,"enum" ),

        new IntProp<BinaryOp>("size", "bits", ""
                         , this, &BinaryOp::size, &BinaryOp::setSize, propSlot ),
    },0} );
}
BinaryOp::~BinaryOp(){}

void BinaryOp::initModule()
{
    m_inputA = 0;
    m_inputB = 0;
    m_output = 0;
}

void BinaryOp::runStep()
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    m_inputA = m_inSlotA.intData() & m_mask;
    m_inputB = m_inSlotB.intData() & m_mask;

    updateOutput();
}

void BinaryOp::updateOutput()
{
    int out = 0;

    switch( m_binOpType ) {
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

int BinaryOp::cmp()
{
    int o = m_inputA - m_inputB;
    if( o  < 0 ) return 1;
    if( o == 0 ) return 2;
    if( o  > 0 ) return 4;
    return -1;
}

void BinaryOp::setTypeStr( QString type )
{
    if( !m_binOpList.contains( type )) type = "AND";
    int t = m_binOpList.indexOf( type );

    m_binOpStr = type;
    m_binOpType = (binOpType_t)t;
}

void BinaryOp::setSize( int bits )
{
    if     ( bits < 1  ) bits = 1;
    else if( bits > 32 ) bits = 32;

    m_bits = bits;
    m_mask = pow( 2, m_bits )-1;
}

