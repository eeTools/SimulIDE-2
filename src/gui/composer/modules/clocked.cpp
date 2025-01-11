/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "clocked.h"
#include "iopin.h"
#include "fblock.h"

QStringList Clocked::m_triggerStr = {"None","Clock","Enable"};

Clocked::Clocked( QString name )
       : Module( name )
       , m_clockSlot("clock", hookInputBit )
{
    m_clock   = false;
    m_clkPin  = nullptr;
    m_trigger = None;
}
Clocked::~Clocked(){}

/*void Clocked::stamp()
{
    if( m_clkPin ){
        m_clock = m_clkPin->inverted();
        m_clkPin->changeCallBack( this );
    }
    else m_clock = false;
}*/

/*bool Clocked::clockInv()
{
    return m_clkPin->inverted();
}

void Clocked::setClockInv( bool inv )
{
    if( !m_clkPin ) return;

    Simulator::self()->pauseSim();

    m_clkPin->setInverted( inv );
    Circuit::self()->update();

    Simulator::self()->resumeSim();
}*/

void Clocked::updateClock()
{
    if( !m_clkPin ) { m_clkState = Clock_Allow; return; }

    m_clkState = Clock_Low;

    bool clock = m_clkPin->getLogicState(); // Clock pin volt.

    if( m_trigger == InEnable )
    {
        if( clock ) m_clkState = Clock_Allow;
    }
    else if( m_trigger == Clock )
    {
        if     (!m_clock &&  clock ) m_clkState = Clock_Rising;
        else if( m_clock &&  clock ) m_clkState = Clock_High;
        else if( m_clock && !clock ) m_clkState = Clock_Falling;
    }
    else m_clkState = Clock_Allow;
    m_clock = clock;
}

void Clocked::setTrigger( trigger_t trigger )
{
    //if( Simulator::self()->isRunning() ) CircuitWidget::self()->powerCircOff();


    m_clock = false;

    if( m_clkPin ){
        if( m_trigger == None )
        {
            m_clkPin->removeWire();
            m_clkPin->setLabelText("");
            m_clkPin->setVisible( false );
        }
        else if( m_trigger == Clock )
        {
            m_clkPin->setLabelText(">");
            m_clkPin->setVisible( true );
        }
        else if( m_trigger == InEnable )
        {
            m_clkPin->setLabelText("IE");
            m_clkPin->setVisible( true );
        }
    }
    //Circuit::self()->update();

    if( m_trigger == trigger ) return;

    if     ( m_trigger && !trigger ) m_slots.pop_back() ;
    else if( !m_trigger && trigger ) m_slots.emplace_back( &m_clockSlot );

    m_trigger = trigger;

    if( m_funcBlock ) m_funcBlock->updateWidget();
}

void Clocked::remove()
{
    if( m_clkPin) m_clkPin->removeWire();
}
/*
QStringList Clocked::getEnumNames( QString property )
{
    if( property == "Trigger") return m_triggerStr;
    else                       return m_enumUids;
}
QStringList Clocked::getEnumUids( QString property )
{
    if( property == "Trigger") return m_triggerStr;
    else                       return m_enumUids;
}*/
