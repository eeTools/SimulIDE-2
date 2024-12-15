/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef PROPERTYM_H
#define PROPERTYM_H

#include "module.h"
//#include "modsignal.h"

class CallBack;

class PropertyM : public Module
{
    public:
        PropertyM( QString name );
        ~PropertyM();

        QString propName() { return m_propName; }
        void setPropName( QString n );

        int value() { return m_value; }
        void setValue( int v );

        int minVal() { return m_minVal; }
        void setMinVal( int v );

        int maxVal() { return m_minVal; }
        void setMaxVal( int v );

        void addCallBack( CallBack* c );

 static moduleItem_t registerItem();

    private:

        QString m_propName;

        int m_value;
        int m_minVal;
        int m_maxVal;

        CallBack* m_outCallback;

        ComProperty* m_valueProp;

 static QString m_moduleType;
};
#endif
