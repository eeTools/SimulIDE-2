/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "m_property.h"
#include "callback.h"
#include "fblock.h"

#include "intprop.h"
#include "stringprop.h"

listItem_t mProperty::registerItem(){
    return {
        "Property",
        "Other",
        "property.png",
        "Property",
        [](int id){ return (CompBase*)new mProperty( id ); } };
}

mProperty::mProperty( int id )
         : Module( id )
{
    m_propName = "Value"+id;
    m_value = 0;
    m_minVal = 0;
    m_maxVal = 999;

    addPropGroup( { "Main",
    {
        new StrProp<mProperty>("name", "Name", ""
                              , this, &mProperty::propName, &mProperty::setPropName,0 ),

        new IntProp<mProperty>("min", "Minimum", ""
                              , this, &mProperty::minVal, &mProperty::setMinVal, 0 ),

        new IntProp<mProperty>("max", "Maximum", ""
                              , this, &mProperty::maxVal, &mProperty::setMaxVal, 0 ),

        new IntProp<mProperty>("propval", "Value", ""
                              , this, &mProperty::value, &mProperty::setValue, propSignal )
    },0} );
}
mProperty::~mProperty(){}

void mProperty::setPropName( QString n )
{
    if( m_funcBlock ) m_funcBlock->renamePropHooks( m_propName, n );
    m_propName = n;
}

void mProperty::setValue( int val )
{
    if( val < m_minVal ) val = m_minVal;
    if( val > m_maxVal ) val = m_maxVal;
    m_value = val;
}

void mProperty::setMinVal( int v )
{
    m_minVal = v;
    setValue( m_value );
}

void mProperty::setMaxVal( int v )
{
    m_maxVal = v;
    setValue( m_value );
}
