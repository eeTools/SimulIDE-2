/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "boolprop.h"
#include "compbase.h"

BoolProp::BoolProp( CompBase* comp, QString idStr, QString label, bool val, uint8_t flags )
        : ComProperty( comp, idStr, label, flags )
{
    m_type = P_Bool;

    m_value.boolVal = val;
    m_defaultVal.boolVal = val;
}
BoolProp::~BoolProp(){}

bool BoolProp::get()
{
    return m_value.boolVal;
}

void BoolProp::set( bool val )
{
    m_value.boolVal = val;
}

void BoolProp::setValStr( QString valStr )
{
    //m_dispValStr = valStr;
    bool boolVal = valStr == "1";
    setValue( {.boolVal=boolVal} );
}

QString BoolProp::getValStr()
{
    QString valStr = m_value.boolVal ? "1" : "0";
    return valStr;
}
