/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "m_config.h"
#include "fcomponent.h"
#include "comproperty.h"
#include "propdialog.h"
#include "simulator.h"

#define tr(str) simulideTr("mConfig",str)

listItem_t mConfig::registerItem(){
    return {
        "Configuration",
        "Other",
        "config.png",
        "mConfig",
        [](int id){ return (CompBase*)new mConfig( id ); } };
}

mConfig::mConfig( int id )
       : Module( id )
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
            QString propId = prop->idStr();
            m_propMap.insert( propId, propLabel );
            compPropStr += propId+",";
            compPropTra += propLabel+",";
        }
    }

    /*addPropGroup( { tr("Main"),
    {
        new StrProp("cfgprop", "Property", compPropStr+";"+compPropTra, this, 0,"enum" ),
        new StrProp("cfgval", "Values", "", this, 0 ),
    },0} );*/
}

void mConfig::setCfgValues( QString v )
{
    m_cfgValues = v;
    m_valueList = v.split(",");
}
