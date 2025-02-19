/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

//#include <QDebug>

#include "doubprop.h"
#include "compbase.h"
#include "utils.h"

DoubProp::DoubProp( CompBase* comp, param_t p, uint8_t idInt )
        : ComProperty( comp, p, idInt )
{
    m_value.dblVal = (double*)p.data;
    m_defaultVal = m_value.dblVal ? *m_value.dblVal : 0;

    m_dispValStr = QString::number( m_defaultVal );
}
DoubProp::~DoubProp(){}

void DoubProp::setValStr( QString valStr )
{
    QStringList words = valStr.split(" ");
    words.removeAll("");
    m_dispValStr = words.first();

    double multiplier = 1;
    if( words.size() > 1 ) // Get multiplier
    {
        QString unit = words.last();
        if( unit.length() > 1 )
        {
            m_multStr = unit.left( 1 );
            multiplier = getMultiplier( m_multStr );
        }
    }

    double value = m_dispValStr.toDouble()*multiplier;

    if( m_flags & P_NoSet ) m_compBase->setValue( m_idInt, {.dblVal=&value} );
    else                    *m_value.dblVal = value;
}

QString DoubProp::getValStr()
{
    QString valStr = m_dispValStr;
    if( !m_unit.isEmpty() ) valStr.append(" "+m_multStr+m_unit );

    return valStr;
}

