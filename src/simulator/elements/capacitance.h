/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef CAPACITANCE_H
#define CAPACITANCE_H

#include "resistance.h"
#include "callback.h"

#include "kcl.h"

class Capacitance : public Resistance
{
    public:
        Capacitance();
        ~Capacitance();

        void stampAdmit() override;
        void stampCurrent() override;
        void voltChanged() override;

        void runStep();

        void setCapacitance( double c ) { m_capacitance = c; }
        double getCapacitance() { return m_capacitance; }

    protected:
        double m_capacitance;
        double m_current0;
        double m_current1;
        double m_timeStep;
        double m_initVolt;
        double m_voltage;

        uint64_t m_reactStep;

        bool m_running;

        int* m_currGroupChg;
        bool* m_currChanged0;
        bool* m_currChanged1;

        SimEvent<Capacitance> m_event; // Callback for simulation event
};

#endif
