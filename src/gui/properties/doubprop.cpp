/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

//#include <QDebug>

#include "doubprop.h"
#include "compbase.h"
#include "utils.h"

DoubProp::DoubProp( CompBase* comp, QString idStr, QString label, QString unit, double val, uint8_t flags )
        : ComProperty( comp, idStr, label, flags )
{
    m_type = P_Double;
    m_unit = unit;

    m_value.dblVal = val;
    m_defaultVal = m_value;

    //m_dispValStr = QString::number( m_defaultVal );
}
DoubProp::~DoubProp(){}

double DoubProp::get()
{
    return m_value.dblVal;
}

void DoubProp::set( double val )
{
    m_value.dblVal = val;
}

void DoubProp::setValStr( QString valStr )
{
    QStringList words = valStr.split(" ");
    words.removeAll("");
    valStr = words.first();

    m_multStr = "";
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

    double value = valStr.toDouble()*multiplier;

    setValue( {.dblVal=value} );
}

QString DoubProp::getValStr() // Returns value + multiplier + unit
{
    QString valStr = QString::number( m_value.dblVal );
    if( !m_unit.isEmpty() ) valStr.append(" "+m_multStr+m_unit );

    return valStr;
}

QString DoubProp::toString() // Returns value + multiplier
{
    QString valStr = QString::number( m_value.dblVal );
    if( !m_multStr.isEmpty() ) valStr.append(" "+m_multStr );
    return valStr;
}

