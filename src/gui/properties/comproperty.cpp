/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

//#include <QFrame>

#include "comproperty.h"
#include "compbase.h"
#include "propwidget.h"
#include "numwidget.h"
#include "boolwidget.h"
#include "strwidget.h"
//#include "enumwidget.h"
//#include "iconwidget.h"
//#include "labelwidget.h"
#include "utils.h"

ComProperty::ComProperty( CompBase* comp, QString idStr, QString label, uint8_t flags )
{
    m_compBase = comp;

    m_idStr = idStr;
    m_label = label;
    //m_unit  = p.unit;
    m_flags = flags;
    m_idInt = comp->newPropId();

    m_widget = nullptr;
}

/*void ComProperty::setValStr( QString valStr )
{
    m_dispValStr = valStr;
}*/

/*QString ComProperty::getValStr()
{
    return m_dispValStr;
}*/


void ComProperty::setValue( value_t val )
{
    m_value = val;
    if( m_flags & P_NoSet ) m_compBase->propertyChanged( this );
}


QString ComProperty::toString(){ return getValStr(); }

/*void ComProperty::setName( QString n )
{
    m_label = n;
    //m_id = n.toLower().replace(" ", "_");
    if( m_widget ) m_widget->updateName();
}*/

void ComProperty::addCallBack( ComProperty* cb )
{
    if( !m_callbacks.contains( cb ) ) m_callbacks.append( cb );
}

PropWidget* ComProperty::getWidget()
{
    if( !m_widget )
    {
        switch( m_type )
        {
        case P_Uint:
        case P_Int:
        case P_Double: m_widget = new NumWidget( nullptr, this );  break;
        case P_Bool:   m_widget = new BoolWidget( nullptr, this ); break;
        //case P_Icon:   m_widget = new IconWidget( nullptr, this ); break;
        case P_Enum:
        case P_Point:
        case P_String: m_widget = new StrWidget( nullptr, this ); break;
        }
    }
    return m_widget;
}
