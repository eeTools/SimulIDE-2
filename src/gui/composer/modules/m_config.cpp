/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "m_config.h"
#include "fcomponent.h"
#include "comproperty.h"
#include "propdialog.h"
#include "simulator.h"

#include "stringprop.h"

#define tr(str) simulideTr("mConfig",str)

listItem_t mConfig::registerItem(){
    return {
        "Configuration",
        "Other",
        "config.png",
        "mConfig",
        [](QString id){ return (CompBase*)new mConfig( id ); } };
}

mConfig::mConfig( QString name )
       : Module( name )
       , m_inputSlot("input", hookInputInt )
{
    m_slots.emplace_back( &m_inputSlot );

    Simulator::self()->addToUpdateList( this );
}
mConfig::~mConfig(){}

void mConfig::initModule()
{
}

void mConfig::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    int value = m_inputSlot.intData();

    QString valStr = m_valueList.at( value );

    m_component->setPropStr( m_cfgProp, valStr );
}

void mConfig::runStep()
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void mConfig::setComponent( fComponent* c )
{
    Module::setComponent( c );

    QString compPropStr;
    QString compPropTra;
    QList<propGroup> groups = m_component->getPropGroups();
    for( propGroup group : groups )
    {
        if( group.name != tr("Package") ) continue;

        for( ComProperty* prop : group.propList )
        {
            QString propLabel = prop->label();
            QString propId = prop->id();
            m_propMap.insert( propId, propLabel );
            compPropStr += propId+",";
            compPropTra += propLabel+",";
        }
    }

    addPropGroup( { tr("Main"),
    {
        new StrProp<mConfig>("cfgprop", "Property", compPropStr+";"+compPropTra
                            , this, &mConfig::cfgProperty, &mConfig::setCfgProperty, 0,"enum" ),

        new StrProp<mConfig>("cfgval", "Values", ""
                            , this, &mConfig::cfgValues, &mConfig::setCfgValues, 0 ),
    },0} );
}

void mConfig::setCfgValues( QString v )
{
    m_cfgValues = v;
    m_valueList = v.split(",");
}
