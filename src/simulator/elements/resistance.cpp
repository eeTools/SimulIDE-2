/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "resistance.h"
#include "kcl.h"

Resistance::Resistance()
          : Element()
{
    m_node0 = -1;
    m_node1 = -1;
}
Resistance::~Resistance(){}

void Resistance::stampAdmit()
{
    m_kcl->addAdmitance( &m_admitance, m_node0, m_node1 );
}

void Resistance::stampCurrent()
{
    m_nodeGroup = m_kcl->getGroup( m_node0 );
}
