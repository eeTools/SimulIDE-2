/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "resistance.h"
#include "kcl.h"

Resistance::Resistance( QString id )
          : Element( id )
{
    m_admitance = 0.01; // Default = 100 Ohm
    m_nodes.resize(2);
    m_nodes[0] = -1;
    m_nodes[1] = -1;
}
Resistance::~Resistance(){}

void Resistance::stampAdmit()
{
    m_node0 = m_nodes[0];
    m_node1 = m_nodes[1];

    m_kcl->addAdmitance( &m_admitance, m_node0, m_node1 );
}

void Resistance::stampCurrent()
{
    m_nodeGroup = m_kcl->getGroup( m_node0 );
}
