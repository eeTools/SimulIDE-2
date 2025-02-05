/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef DOUBPROP_H
#define DOUBPROP_H

#include "numprop.h"
#include "numwidget.h"

template <class Comp>
class DoubProp : public NumProp
{
    public:
        DoubProp( QString id, QString label, QString unit, Comp* comp
                , double (Comp::*getter)(), void (Comp::*setter)(double)
                , uint8_t flags=0, QString type="double" )
        : NumProp( id, label, unit, type, flags )
        {
            m_comp = comp;
            m_getter = getter;
            m_setter = setter;
        }
        ~DoubProp(){;}

        virtual double getValue()
        { return (m_comp->*m_getter)(); }

    private:
        void createWidget() override { m_widget = new NumWidget( nullptr, m_comp, this ); }
        void setVal( double v ) override { (m_comp->*m_setter)(v); }

        Comp* m_comp;
        double (Comp::*m_getter)();
        void   (Comp::*m_setter)(double);
};

#endif
