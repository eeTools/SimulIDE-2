/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "m_dial.h"
#include "fblock.h"

listItem_t mDial::registerItem(){
    return {
        "Dial",
        "Controls",
        "dial.png",
        "mDial",
        [](QString id){ return (CompBase*)new mDial( id ); } };
}

mDial::mDial( QString name )
     : Module( name )
     , m_outSignal("output", hookOutputInt )
{

    m_outSignal.setIntData( &m_output );
    m_signals.emplace_back( &m_outSignal );
}
mDial::~mDial(){}

void mDial::initModule()
{
    m_output = 0;
}

void mDial::runStep()
{
    if( !m_changed ) return;
    m_changed = false;

    int out = 0;

    if( m_output == out ) return;
    m_output = out;
    m_outSignal.changed();
}
