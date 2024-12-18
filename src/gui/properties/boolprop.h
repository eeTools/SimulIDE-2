/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BOOLPROP_H
#define BOOLPROP_H

#include "comproperty.h"
#include "boolwidget.h"

template <class Comp>
class BoolProp : public ComProperty
{
    public:
        BoolProp( QString name, QString caption, QString unit, Comp* comp
                , bool (Comp::*getter)(), void (Comp::*setter)(bool)
                , uint8_t flags=0, QString type="bool" )
        : ComProperty( name, caption, unit, type, flags )
        {
            m_comp = comp;
            m_getter = getter;
            m_setter = setter;
        }
        ~BoolProp(){;}

        virtual void setValStr( QString val ) override
        { (m_comp->*m_setter)( val == "1" ); }

        virtual QString getValStr() override
        { return (m_comp->*m_getter)() ? "1" : "0"; }

    private:
        void createWidget() override { m_widget = new BoolWidget( nullptr, m_comp, this ); }

        Comp* m_comp;
        bool (Comp::*m_getter)();
        void (Comp::*m_setter)(bool);
};

#endif
