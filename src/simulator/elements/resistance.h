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

        void stampAdmit() override;
        void stampCurrent() override;

        double getResistance() { return 1/m_admitance;}
        void setResistance( double r ) { m_admitance = 1/r; }

        double m_admitance;

        int m_node0;
        int m_node1;

    protected:
        //double m_current;

        int m_nodeGroup;
};
