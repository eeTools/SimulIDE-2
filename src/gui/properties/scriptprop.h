/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef SCRIPTPROP_H
#define SCRIPTPROP_H

#include "comproperty.h"
#include "strwidget.h"

template <class Comp>
class ScriptProp : public ComProperty
{
    public:
        ScriptProp( QString id, QString label, QString unit, Comp* comp
                  , QString (Comp::*getter)(ComProperty*), void (Comp::*setter)(ComProperty*, QString)
                  , QString type="string", uint8_t flags=0 )
        : ComProperty( id, label, unit, type, flags )
        {
            m_comp = comp;
            m_getter = getter;
            m_setter = setter;
        }
        ~ScriptProp(){;}

        virtual void setValStr( QString val ) override
        { (m_comp->*m_setter)( this, val /*setStr( val )*/ ); } // Call setter with property name

        virtual QString getValStr() override
        { return (m_comp->*m_getter)(this); }

        PropWidget* createWidget() override { return new StrWidget( nullptr, m_comp, this ); }

    private:
        Comp* m_comp;
        QString (Comp::*m_getter)(ComProperty*);
        void    (Comp::*m_setter)(ComProperty*, QString);
};

#endif
