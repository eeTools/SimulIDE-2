/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "m_property.h"
#include "callback.h"
#include "fblock.h"

#include "intprop.h"
#include "stringprop.h"

listItem_t PropertyM::registerItem(){
    return {
        "Property",
        "Other",
        "property.png",
        "Property",
        [](QString id){ return (CompBase*)new PropertyM( id ); } };
}

PropertyM::PropertyM( QString id )
         : Module( id )
{
    m_outCallback = nullptr;
    m_propName = "Value"+id;
    m_value = 0;
    m_minVal = 0;
    m_maxVal = 999;

    addPropGroup( { "Main",
    {
        new StrProp<PropertyM>("name", "Name", ""
                              , this, &PropertyM::propName, &PropertyM::setPropName,0 ),

        new IntProp<PropertyM>("min", "Minimum", ""
                              , this, &PropertyM::minVal, &PropertyM::setMinVal, 0 ),

        new IntProp<PropertyM>("max", "Maximum", ""
                              , this, &PropertyM::maxVal, &PropertyM::setMaxVal, 0 ),

        new IntProp<PropertyM>("propval", "Value", ""
                              , this, &PropertyM::value, &PropertyM::setValue, propSignal )
    },0} );
}
PropertyM::~PropertyM(){}

void PropertyM::setPropName( QString n )
{
    if( m_funcBlock ) m_funcBlock->renamePropHooks( m_propName, n );
    m_propName = n;
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
    m_maxVal = v;
    setValue( m_value );
}

void PropertyM::addCallBack( CallBack* c ) /// TODO: Maibe not needed
{
    c->next = m_outCallback;
    m_outCallback = c;
}
