/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

//#include <QFrame>

#include "comproperty.h"
#include "propwidget.h"
#include "labelwidget.h"
#include "utils.h"

ComProperty::ComProperty( QString id, QString label, QString unit, QString type, uint8_t flags )
{
    m_id    = id;
    m_label = label;
    m_unit  = unit;
    m_type  = type;
    m_flags = flags;

    m_widget = nullptr;
}
QString ComProperty::id()   { return m_id; }
QString ComProperty::label(){ return m_label; }
QString ComProperty::type() { return m_type; }
QString ComProperty::unit() { return m_unit; }
uint8_t ComProperty::flags(){ return m_flags; }

void    ComProperty::setValStr( QString ){;}
QString ComProperty::getValStr(){ return ""; }
double  ComProperty::getValue(){ return getValStr().toDouble(); }

QString ComProperty::toString(){ return getValStr(); }

/*void ComProperty::setName( QString n )
{
    m_label = n;
    //m_id = n.toLower().replace(" ", "_");
    if( m_widget ) m_widget->updateName();
}*/

void ComProperty::addCallBack( ComProperty* cb )
{
    /// if( !m_signal ) m_signal = new ModSignal( "signal@"+m_id, hookProperty );
    /// m_signal->connect( cb );
    if( !m_callbacks.contains( cb ) ) m_callbacks.append( cb );
}

PropWidget* ComProperty::getWidget()
{
    if( !m_widget ) createWidget();
    return m_widget;
}
