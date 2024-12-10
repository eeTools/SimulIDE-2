/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "element.h"
#include "simulator.h"
//#include "kcl.h"

Element::Element( QString id )
{
    m_elId = id;
    m_kcl = Kcl::self();

    Simulator::self()->addElement( this );
}
Element::~Element()
{
    Simulator::self()->remElement( this );
}
