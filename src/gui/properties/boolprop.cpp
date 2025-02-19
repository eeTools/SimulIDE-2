/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "boolprop.h"
#include "compbase.h"

BoolProp::BoolProp( CompBase* comp, param_t p, uint8_t idInt )
        : ComProperty( comp, p, idInt )
{
    m_value.boolVal = (bool*)   p.data;
    if( m_value.boolVal ) m_defaultVal = *m_value.boolVal;
}
BoolProp::~BoolProp(){}

void BoolProp::setValStr( QString valStr )
{
    m_dispValStr = valStr;
    bool boolVal = valStr == "1"; /// FIXME valStr;

    if( m_value.boolVal ) *m_value.boolVal = boolVal;
    else                  m_compBase->setValue( m_idInt, {.boolVal=&boolVal} );
}
