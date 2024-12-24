/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef INTPROPF_H
#define INTPROPF_H

#include "comproperty.h"
#include "numwidget.h"

class IntPropF : public ComProperty
{
    public:
        IntPropF( QString id, QString label, QString unit, CompBase* comp
                , uint8_t flags=0, QString type="int" )
        : ComProperty( id, label, unit, type, flags )
        {
            m_component = comp;

            m_value  = 2; /// TODO: setters
            m_minVal = 2;
            m_maxVal = 32;
        }
        ~IntPropF(){;}

        QString getValStr()  override
        { return QString::number( m_value ); }

        void setValStr( QString valStr ) override
        {
            int val = valStr.toInt();
            if( val < m_minVal ) val = m_minVal;
            if( val > m_maxVal ) val = m_maxVal;

            for( ComProperty* cb : m_callbacks ) cb->setValStr( QString::number( val ) );

            m_value = val;
        }

        virtual double getValue() override
        { return m_value; }

    private:
        void createWidget() override { m_widget = new NumWidget( nullptr, m_component, this ); }

        int m_value;
        int m_minVal;
        int m_maxVal;

        CompBase* m_component;
};

#endif
