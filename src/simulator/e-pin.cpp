/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "e-pin.h"
#include "kcl.h"

ePin::ePin( QString id )
{
    m_id    = id;
    m_enode = -1;
    m_inverted = false;
}
ePin::~ePin(){}

double ePin::getVoltage()
{
    if( isConnected() ) return Kcl::self()->getVoltage( m_enode );
    return 0;
}
