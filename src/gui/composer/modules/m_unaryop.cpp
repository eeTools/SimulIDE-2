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
        [](int id){ return (CompBase*)new UnaryOp( id ); } };
}

UnaryOp::UnaryOp( int id )
       : Clocked( id )
       , m_inputSlot("input", hookInputInt )
       , m_outSignal("output", hookOutputInt )
{
    m_slots.emplace_back( &m_inputSlot );
    m_signals.emplace_back( &m_outSignal );

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

void UnaryOp::setTypeStr( QString type )
{
    if( !m_unOpList.contains( type )) type = "AND";
    m_unOpStr = type;
}

void UnaryOp::setSize( int bits )
{
    if     ( bits < 1  ) bits = 1;
    else if( bits > 32 ) bits = 32;

    m_bits = bits;
}

