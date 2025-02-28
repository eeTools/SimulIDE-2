/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "strprop.h"
#include "compbase.h"

StrProp::StrProp( CompBase* comp, QString idStr, QString label, QString val, uint8_t flags )
       : ComProperty( comp, idStr, label, flags )
{
    m_type = P_String;
    m_strVal = val;

    m_value.strVal = &m_strVal;
    m_defaultVal = m_value;
}
StrProp::~StrProp(){}

void StrProp::setValStr( QString valStr )
{
    m_strVal = valStr;
    if( m_flags & P_NoSet ) m_compBase->propertyChanged( this );
}

QString StrProp::getValStr()
{
    return m_strVal;
}
