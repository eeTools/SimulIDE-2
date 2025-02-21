/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

//#include <QStringList>

#include "m_delay.h"
#include "simulator.h"
#include "fcomponent.h"

listItem_t Delay::registerItem(){
    return {
        "Delay",
        "Other",
        "delay.png",
        "Delay",
        [](int id){ return (CompBase*)new Delay( id ); } };
}

Delay::Delay( int id )
     : Module( id )
     , m_inputSlot("input", hookInputInt )
     , m_outSignal("output", hookOutputInt )
{
    m_delay = 10000;         // 10 ns

    m_outSignal.setIntData( &m_outState );

    m_slots.emplace_back( &m_inputSlot );
    m_signals.emplace_back( &m_outSignal );

    /*addPropGroup( { "Main",
    {
        new DoubProp("Time", "Time", "s", this )
    },0} );*/
}
Delay::~Delay(){}

void Delay::initModule()
{
    m_freeEvent = nullptr;
}

void Delay::runStep()
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    m_outState = m_inputSlot.intData();

    if( !m_freeEvent )
    {
        m_freeEvent = new SimEventInt<Delay>( this, &Delay::runOutput );
    }
    else{
        uint64_t time = Simulator::self()->circTime()+m_delay;
        if( time == m_freeEvent->time ){
            m_freeEvent->valInt = m_delay;
            return;
        }
        m_freeEvent = m_freeEvent->nextInt;
    }
    m_freeEvent->valInt = m_delay;
    Simulator::self()->addEvent( m_delay, m_freeEvent );
}

void Delay::runOutput( SimEventInt<Delay>* event )
{
    event->nextInt = m_freeEvent;
    m_freeEvent    = event;        // Free this event

    m_outSignal.changed();
    m_component->voltChanged();
}

