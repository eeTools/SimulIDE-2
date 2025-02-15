/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef INTPROP_H
#define INTPROP_H

#include "numprop.h"
#include "numwidget.h"

template <class Comp>
class IntProp : public NumProp
{
    public:
        IntProp( QString id, QString label, QString unit, Comp* comp
               , int (Comp::*getter)(), void (Comp::*setter)(int)
               , uint8_t flags=0, QString type="int" )
        : NumProp( id, label, unit, type, flags )
        {
            m_comp = comp;
            m_getter = getter;
            m_setter = setter;
        }
        ~IntProp(){;}

        double getValue() override
        { return (m_comp->*m_getter)(); }

    private:
        void createWidget() override { m_widget = new NumWidget( nullptr, m_comp, this ); }
        void setVal( double v ) override { (m_comp->*m_setter)(v); }

        Comp* m_comp;
        int  (Comp::*m_getter)();
        void (Comp::*m_setter)(int);
};

#endif
