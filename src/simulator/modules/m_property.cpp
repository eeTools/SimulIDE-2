/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "m_property.h"
#include "callback.h"

#include "intprop.h"
#include "stringprop.h"

QString PropertyM::m_moduleType = "Property";

moduleItem_t PropertyM::registerItem(){
    return {
        "Property",
        m_moduleType,
        "",
        "property.png",
        [](QString id){ return (Module*)new PropertyM( id ); } };
}

PropertyM::PropertyM( QString name )
         : Module( name )
         //, m_inputSlot("input", hookInputInt/*, &m_changed*/ )
         //, m_outSignal("output", hookOutputInt )
{
    m_type = m_moduleType;

    m_outCallback = nullptr;
    m_propName = "Value";
    m_value = 0;
    m_valueProp = new IntProp<PropertyM>("value", "Value", ""
                                        , this, &PropertyM::value, &PropertyM::setValue, propSignal );
    addPropGroup( { "Main",
    {
        new StrProp<PropertyM>("name", "Name", ""
                              , this, &PropertyM::propName, &PropertyM::setPropName,0 ),
        m_valueProp,

        new IntProp<PropertyM>("min", "Minimum", ""
                              , this, &PropertyM::minVal, &PropertyM::setMinVal, 0 ),

        new IntProp<PropertyM>("max", "Maximum", ""
                              , this, &PropertyM::maxVal, &PropertyM::setMaxVal, 0 )
    },0} );
}
PropertyM::~PropertyM(){}

void PropertyM::setPropName( QString n )
{
    m_propName = n;
    m_valueProp->setName( n );
}

void PropertyM::setValue( int val )
{
    if( val < m_minVal ) val = m_minVal;
    if( val > m_maxVal ) val = m_maxVal;
    m_value = val;

    CallBack* cb = m_outCallback;  /// TODO: Maibe not needed
    while( cb ){
        cb->call( val );
        cb = cb->next;
    }
}

void PropertyM::setMinVal( int v )
{
    m_minVal = v;
    setValue( m_value );
}

void PropertyM::setMaxVal( int v )
{
    m_minVal = v;
    setValue( m_value );
}

void PropertyM::addCallBack( CallBack* c ) /// TODO: Maibe not needed
{
    c->next = m_outCallback;
    m_outCallback = c;
}
