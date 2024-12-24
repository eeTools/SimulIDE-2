/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef UINTPROP_H
#define UINTPROP_H

#include "numprop.h"
#include "numwidget.h"

template <class Comp>
class UintProp : public NumProp
{
    public:
        UintProp( QString id, QString label, QString unit, Comp* comp
                , uint64_t (Comp::*getter)(), void (Comp::*setter)(uint64_t)
                , uint8_t flags=0, QString type="uint" )
        : NumProp( id, label, unit, type, flags )
        {
            m_comp   = comp;
            m_getter = getter;
            m_setter = setter;
        }
        ~UintProp(){;}

        double getValue() override
        { return (m_comp->*m_getter)(); }

    private:
        void createWidget() override { m_widget = new NumWidget( nullptr, m_comp, this ); }
        void setVal( double v ) override { (m_comp->*m_setter)(v); }

        Comp* m_comp;
        uint64_t (Comp::*m_getter)();
        void (Comp::*m_setter)(uint64_t);
};

#endif
