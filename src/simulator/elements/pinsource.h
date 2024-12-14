/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef PINSOURCE_H
#define PINSOURCE_H

#include "element.h"

class PinSource : public Element
{
    public:
        PinSource( QString id );
        ~PinSource();

        void stampAdmit() override;
        void stampCurrent() override;

        void setNode( int n );

        double getVoltage();
        void setVoltage( double v ) { m_voltage = v; }
        void updtVoltage( double v );

        void setAdmitance( double a ) { m_admitance = a; }
        void updtAdmitance( double a );

    protected:
        inline void updateCurrent()
        {
            m_current = m_voltage*m_admitance;
            *m_currGroupChg = true;
            *m_currChanged = true;
        }
        double m_voltage;
        double m_admitance;
        double m_current;

        int m_node;
        int m_nodeGroup;

        int*  m_currGroupChg;
        bool* m_currChanged;
};

#endif
