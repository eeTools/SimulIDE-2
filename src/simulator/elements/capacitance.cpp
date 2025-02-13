/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

// Capacitance companion model using backwards Euler aproximation
// Not so precise but more stable

#include "capacitance.h"
#include "simulator.h"

Capacitance::Capacitance()
           : Resistance()
           , m_event( this, &Capacitance::runStep )
{
    m_initVolt = 0;
    m_capacitance = 0.00001; // Farads
}
Capacitance::~Capacitance(){}

void Capacitance::stampAdmit()
{
    m_running = false;

    m_reactStep = Simulator::self()->reactStep();
    m_timeStep = (double)m_reactStep/1e12;

    Resistance::setResistance( m_timeStep/m_capacitance );
    Resistance::stampAdmit();
}

void Capacitance::stampCurrent()
{
    Resistance::stampCurrent(); // Nodegroup retrieved here

    m_voltage  = m_initVolt;
    m_current0 = m_voltage*m_admitance;
    m_current1 = -m_current0;

    m_currChanged0 = m_kcl->addCurrent( &m_current0, m_node0 );
    m_currChanged1 = m_kcl->addCurrent( &m_current1, m_node1 );

    m_kcl->addChangeCB( this, m_node0 );
    m_kcl->addChangeCB( this, m_node1 );

    m_currGroupChg = m_kcl->getCurrentGroupChanged( m_nodeGroup );

    /// TODO: call Kcl to mark group to not precalculate.
}

void Capacitance::runStep()
{
    double voltage = m_kcl->getVoltage( m_node0 ) - m_kcl->getVoltage( m_node1 );

    if( m_voltage != voltage )
    {
        m_voltage  = voltage;
        m_current0 = voltage*m_admitance;
        m_current1 = -m_current0;

        *m_currGroupChg = true;
        *m_currChanged0 = true;
        *m_currChanged1 = true;

        Simulator::self()->addEvent( m_reactStep, &m_event );
    }else{
        m_running = false;
        m_kcl->addChangeCB( this, m_node0 );
        m_kcl->addChangeCB( this, m_node1 );
    }
}

void Capacitance::voltChanged()
{
    if( m_running ) return;
    m_running = true;

    m_kcl->remChangeCB( this, m_node0 );
    m_kcl->remChangeCB( this, m_node1 );

    Simulator::self()->addEvent( m_reactStep, &m_event );
}
