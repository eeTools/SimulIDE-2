/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "intprop.h"
#include "compbase.h"

IntProp::IntProp( CompBase* comp, QString idStr, QString label, int64_t val, uint8_t flags )
       : ComProperty( comp, idStr, label, flags )
{
    m_type = P_Int;

    m_value.intVal  = val;
    m_defaultVal = m_value;

    //m_dispValStr = QString::number( m_defaultVal );
}
IntProp::~IntProp(){}


void IntProp::setValStr( QString valStr )
{
    //m_dispValStr = valStr;
    int64_t intVal = valStr.toLongLong();
    if( m_type == P_Uint && intVal < 0 ) intVal = 0; /// FIXME: set zero or return??

    setValue( {.intVal=intVal} );
}

QString IntProp::getValStr()
{
    QString valStr = QString::number( m_value.intVal );

    return valStr;
}
