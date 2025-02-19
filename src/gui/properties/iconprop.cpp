/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "iconprop.h"
#include "compbase.h"

IconProp::IconProp( CompBase* comp, param_t p, uint8_t idInt )
        : ComProperty( comp, p, idInt )
{
    m_value.boolVal = (bool*)   p.data;
    m_defaultVal = *m_value.boolVal;
}
IconProp::~IconProp(){}

void IconProp::setValStr( QString valStr )
{
    m_dispValStr = valStr;

    if( m_value.strVal ) *m_value.strVal = valStr;
    else                 m_compBase->setValue( m_idInt, {.strVal=&valStr} );
}
