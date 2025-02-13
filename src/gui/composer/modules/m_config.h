/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "module.h"
#include "updatable.h"
#include "modsignal.h"
#include "callback.h"

class mConfig : public Module, public Updatable
{
    public:
        mConfig( int id );
        ~mConfig();

 static listItem_t registerItem();

         void initModule() override;
         void runStep() override;
         void updateStep() override;

         void setComponent( fComponent* c ) override;

         QString cfgProperty() { return m_cfgProp; }
         void setCfgProperty( QString p ) { m_cfgProp = p; }

         QString cfgValues() { return m_cfgValues; }
         void setCfgValues( QString v );

    private:

         QString m_cfgProp;
         QMap<QString, QString> m_propMap;

         QString m_cfgValues;
         QStringList m_valueList;

         ModSlot m_inputSlot;
};
