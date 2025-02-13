/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QStringList>

#include "m_binaryop.h"
#include "fblock.h"

#include "intprop.h"
#include "stringprop.h"

//#define tr(str) simulideTr("BinaryOp",str)

listItem_t BinaryOp::registerItem(){
    return {
        "Binary Operation",
        "Operations",
        "gate.png",
        "BinaryOp",
        [](int id){ return (CompBase*)new BinaryOp( id ); } };
}

BinaryOp::BinaryOp( int id )
        : Module( id )
        , m_inSlotA("0", hookInputInt )
        , m_inSlotB("1", hookInputInt )
        , m_outSignal("output", hookOutputInt )
{
    //m_binOpType = AND;
    m_binOpStr = "AND";
    m_bits = 2;

    /// m_outSignal.setIntData( &m_output );

    m_slots.emplace_back( &m_inSlotA );
    m_slots.emplace_back( &m_inSlotB );
    m_signals.emplace_back( &m_outSignal );

    m_binOpList = QStringList()
        <<"AND"<<"OR"<<"XOR"
        <<"+"<<"-"
        <<"x"<<"/"
        <<"%"<<"<=>";

    /// TODO: Add Clock

    addPropGroup( { "Main",
    {
        new StrProp<BinaryOp>("op", "Operation", m_binOpList.join(",")
                         , this, &BinaryOp::typeStr, &BinaryOp::setTypeStr,propSlot,"enum" ),

        new IntProp<BinaryOp>("size", "bits", ""
                         , this, &BinaryOp::size, &BinaryOp::setSize, propSlot ),
    },0} );
}
BinaryOp::~BinaryOp(){}

void BinaryOp::setTypeStr( QString type )
{
    if( !m_binOpList.contains( type ) ) type = "AND";
    m_binOpStr = type;
}

void BinaryOp::setSize( int bits )
{
    if     ( bits < 1  ) bits = 1;
    else if( bits > 32 ) bits = 32;

    m_bits = bits;
}

