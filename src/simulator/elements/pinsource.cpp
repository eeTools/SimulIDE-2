/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "pinsource.h"
#include "kcl.h"

PinSource::PinSource( QString id )
         : Element( id )
{
    m_nodes.resize( 1,-1 );
    m_node = -1;

    m_admitance = 0;
    m_currChanged  = nullptr;
    m_currGroupChg = nullptr;
}
PinSource::~PinSource(){}

void PinSource::stampAdmit()
{
    m_voltage = 0;

    m_node = m_nodes[0];
    if( m_node == -1 ) return;  // Not connected

    m_kcl->addAdmitance( &m_admitance, m_node );
}

void PinSource::stampCurrent()
{
    if( m_node == -1 ) return; // Not connected

    m_nodeGroup = m_kcl->getGroup( m_node );

    m_current = m_voltage*m_admitance;
    m_currChanged  = m_kcl->addCurrent( &m_current, m_node );
    m_currGroupChg = m_kcl->getCurrentGroupChanged( m_nodeGroup );
}

void PinSource::updtVoltage( double v )
{
    if( m_node == -1 ) return; // Not connected

    if( m_voltage == v ) return;
    m_voltage = v;

    m_current = m_voltage*m_admitance;
    *m_currGroupChg = true;
    *m_currChanged = true;
}

double PinSource::getVoltage()
{
    if( m_node == -1 ) return 0;
    return m_kcl->getVoltage( m_node );
}

void PinSource::updtAdmitance( double a )
{
    if( m_node == -1 ) return; // Not connected

    if( m_admitance == a ) return;
    m_admitance = a;

    m_kcl->admitChanged( m_node, -1, m_nodeGroup );
}

void PinSource::setNode( int n )
{
    m_nodes[0] = n;
}
