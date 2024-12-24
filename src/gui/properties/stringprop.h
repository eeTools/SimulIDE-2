/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef STRPROP_H
#define STRPROP_H

#include "comproperty.h"
#include "strwidget.h"
#include "enumwidget.h"
#include "iconwidget.h"

template <class Comp>
class StrProp : public ComProperty
{
    public:
        StrProp( QString id, QString label, QString unit, Comp* comp
               , QString (Comp::*getter)(), void (Comp::*setter)(QString)
               , uint8_t flags=0, QString type="string" )
        : ComProperty( id, label, unit, type, flags )
        {
            m_comp = comp;
            m_getter = getter;
            m_setter = setter;
        }
        ~StrProp(){;}

        virtual void setValStr( QString val ) override
        { (m_comp->*m_setter)( val ); } // Comp setter can change valLabel

        virtual QString getValStr() override
        { return (m_comp->*m_getter)(); }

    private:
        void createWidget() override
        {
            if( m_type == "string"  ) m_widget = new StrWidget ( nullptr, m_comp, this );
            if( m_type == "enum"    ) m_widget = new EnumWidget( nullptr, m_comp, this );
            if( m_type == "icon"    ) m_widget = new IconWidget( nullptr, m_comp, this );
        }

        Comp* m_comp;
        QString (Comp::*m_getter)();
        void    (Comp::*m_setter)(QString);
};

#endif
