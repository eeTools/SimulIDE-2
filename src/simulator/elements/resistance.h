/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "element.h"

class Resistance : public Element
{
    public:
        Resistance();
        ~Resistance();

        virtual void stampAdmit() override;
        virtual void stampCurrent() override;

        double getResistance() { return 1/m_admitance;}
        void setResistance( double r ) { m_admitance = 1/r; }
        void setAdmitance( double a )  { m_admitance = a; }

    protected:
        double m_admitance;
        double m_current;

        int m_node0;
        int m_node1;
        int m_nodeGroup;
};
