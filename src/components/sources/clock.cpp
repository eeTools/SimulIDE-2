/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "clock.h"
#include "iopin.h"
#include "simulator.h"

#include "doubleprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("Clock",str)

listItem_t Clock::libraryItem()
{
    return {
        tr("Clock"),
        "Sources",
        "clock.png",
        "Clock",
        [](QString id){ return (Component*)new Clock( id ); } };
}

Clock::Clock( QString id )
     : ClockBase( id )
{
    remPropGroup( tr("Main") );

    addPropGroup( { tr("Main"), {
        new DoubProp<Clock>("Voltage", tr("Voltage"), "V"
                           , this, &Clock::voltage, &Clock::setVoltage ),

        new DoubProp<Clock>("Freq", tr("Frequency"),"kHz"
                           , this, &Clock::freq, &Clock::setFreq ),

        new BoolProp<Clock>("Always_On", tr("Always On"), ""
                           , this, &Clock::alwaysOn, &Clock::setAlwaysOn ),
    }, 0} );
}
Clock::~Clock(){}

void Clock::runEvent()
{
    m_state = !m_state;
    m_outPin->setOutState( m_state );

    m_remainder += m_fstepsPC-(double)m_stepsPC;
    uint64_t remainerInt = m_remainder;
    m_remainder -= remainerInt;

    if( m_isRunning ) Simulator::self()->addEvent( m_stepsPC/2+remainerInt, &m_event );
}

void Clock::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    if( m_hidden ) return;

    Component::paint( p, o, w );

    if (  m_isRunning ) p->setBrush( QColor( 250, 200, 50 ) );
    else                p->setBrush( QColor( 230, 230, 255 ) );

    p->drawRoundedRect( m_area,2 ,2 );

    QPen pen;
    pen.setWidth(1);
    p->setPen( pen );
    
    p->drawLine(-11, 3,-11,-3 );
    p->drawLine(-11,-3,-5, -3 );
    p->drawLine( -5,-3,-5,  3 );
    p->drawLine( -5, 3, 1,  3 );
    p->drawLine(  1, 3, 1, -3 );
    p->drawLine(  1,-3, 4, -3 );

    Component::paintSelected( p );
}
