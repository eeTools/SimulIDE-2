/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QStringList>
#include <math.h>

#include "m_floatop.h"
#include "fblock.h"

#include "intprop.h"
#include "stringprop.h"

#define tr(str) simulideTr("FloatOpOp",str)

listItem_t FloatOp::registerItem(){
    return {
            "Float Operation",
            "Operations",
            "gate.png",
            "FloatOp",
            [](QString id){ return (CompBase*)new FloatOp( id ); } };
}

FloatOp::FloatOp( QString name )
    : Module( name )
    , m_inSlotA("inputA", hookInputDoub )
    , m_inSlotB("inputB", hookInputDoub )
    , m_outSignal("output", hookOutputDoub )
{
    m_floatOpType = ADD;

    m_outSignal.setDblData( &m_output );

    m_slots.emplace_back( &m_inSlotA );
    m_slots.emplace_back( &m_inSlotB );
    m_signals.emplace_back( &m_outSignal );

    m_floatOpList = QStringList() <<"Add"<<"Substract"
                                <<"Multiply"<<"Divide"
                                <<"Compare";

    QString trOpList = tr("Add")+","+tr("Substract")
                       +","+tr("Multiply")+","+tr("Divide")
                       +","+tr("Compare");

    /// TODO: Add Clock

    addPropGroup( { "Main",
    {
        new StrProp<FloatOp>("gType", "Type", m_floatOpList.join(",")+";"+trOpList
                            , this, &FloatOp::typeStr, &FloatOp::setTypeStr,propSlot,"enum" ),
    },0} );
}
FloatOp::~FloatOp(){}

void FloatOp::initModule()
{
    m_inputA = 0;
    m_inputB = 0;
    m_output = 0;
}

void FloatOp::runStep()
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    m_inputA = m_inSlotA.dblData();
    m_inputB = m_inSlotB.dblData();

    updateOutput();
}

void FloatOp::updateOutput()
{
    double out;

    switch( m_floatOpType ) {
    case ADD: out = m_inputA + m_inputB; break;
    case SUB: out = m_inputA - m_inputB; break;
    case MUL: out = m_inputA * m_inputB; break;
    case DIV: out = m_inputA / m_inputB; break;
    case CMP: out = cmp();               break;
    }

    if( m_output == out ) return;
    m_output = out;
    m_outSignal.changed();
}

double FloatOp::cmp()
{
    double o = m_inputA - m_inputB;
    if( o  < 0 ) return 1;
    if( o == 0 ) return 2;
    if( o  > 0 ) return 4;
    return -1;
}

void FloatOp::setTypeStr( QString type )
{
    if( !m_floatOpList.contains( type )) type = "ADD";
    int t = m_floatOpList.indexOf( type );

    m_floatOpStr = type;
    m_floatOpType = (floatOpType_t)t;
}
