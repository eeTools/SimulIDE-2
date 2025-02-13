/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "pinsource.h"

PinSource::PinSource( QString id, int* node )
         : Element()
{
    m_node = node;
    m_admitance = -1;
    m_currChanged  = nullptr;
    m_currGroupChg = nullptr;
}
PinSource::~PinSource(){}

void PinSource::stampAdmit()
{
    if( *m_node < 0 || m_admitance < 0 ) return;

    m_kcl->addAdmitance( &m_admitance, *m_node );
}

void PinSource::stampCurrent()
{
    if( *m_node < 0 ) return; // Not connected

    m_nodeGroup = m_kcl->getGroup( *m_node );

    m_currChanged  = m_kcl->addCurrent( &m_current, *m_node );
    m_currGroupChg = m_kcl->getCurrentGroupChanged( m_nodeGroup );
}
