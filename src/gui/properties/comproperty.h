/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QString>
#include <QList>

#include "proptypes.h"

class PropWidget;
class CompBase;

class ComProperty
{
    friend class PropWidget;

    public:
        ComProperty( CompBase* comp, QString idStr, QString label, uint8_t flags );
        ~ComProperty(){;}

        uint8_t idInt() { return m_idInt; }
        QString idStr() { return m_idStr; }
        QString label() { return m_label; }
        QString unit()  { return m_unit; }
        pType_t type()  { return (pType_t)m_type; }
        flags_t flags() { return (flags_t)m_flags; }

        void setDefault() { setValue( m_defaultVal ); }

        value_t value() { return m_value; }
        void setValue( value_t val );

        //void setName( QString n );

        virtual void    setValStr( QString ){;}
        virtual QString getValStr(){ return "";}

        virtual QString toString();

        void addCallBack( ComProperty* cb );

        //CompBase* compBase() { return m_compBase; }

        virtual PropWidget* getWidget();

    protected:
        //virtual void createWidget(){;}

        QList<ComProperty*> m_callbacks;

        PropWidget* m_widget;

        CompBase* m_compBase;

        QString m_idStr;
        QString m_label;

        value_t m_value;
        value_t m_defaultVal;

        QString m_unit;
        uint8_t m_type;
        uint8_t m_flags;
        uint8_t m_idInt;
};
