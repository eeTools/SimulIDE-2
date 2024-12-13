/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "clock-base.h"
#include "iopin.h"
#include "circuit.h"
#include "simulator.h"
#include "custombutton.h"

#include "boolprop.h"

ClockBase::ClockBase( QString id )
         : Component( id )
         , m_event( this, &ClockBase::runEvent )
{
    m_area = QRect(-14,-8, 22, 16 );

    m_graphical = true;
    m_isRunning = false;
    m_alwaysOn  = false;

    m_outPin = new IoPin( 0, QPoint(16,0), "outPin@"+id, this, source );
    m_pin.emplace_back( m_outPin );

    setValLabelPos(-16, 8 , 0 ); // x, y, rot
    setLabelPos(-64,-24 );

    m_button = new CustomButton();
    m_button->setMaximumSize( 16,16 );
    m_button->setGeometry(-20,-16,16,16);
    m_button->setCheckable( true );

    m_proxy = Circuit::self()->addWidget( m_button );
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint(-32, -8) );

    setVoltage( 5.0 );

    m_stepsPC = 0;
    ClockBase::setFreq( 1000 );

    Simulator::self()->addToUpdateList( this );

    QObject::connect( m_button, &CustomButton::clicked, [=](){ onbuttonclicked(); } );

    addPropGroup( { "Hidden1", {
        new BoolProp<ClockBase>("Running","",""
                               , this, &ClockBase::running, &ClockBase::setRunning ),
    }, groupHidden} );
}
ClockBase::~ClockBase(){}

void ClockBase::initialize()
{
    setFreq( m_freq );
}

void ClockBase::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    bool running = m_button->isChecked();

    if( running && !m_isRunning )
    {
        //m_lastTime = Simulator::self()->circTime();
        Simulator::self()->addEvent( 0, &m_event );
    }
    m_isRunning = running;
    update();
}

void ClockBase::setAlwaysOn( bool on )
{
    m_alwaysOn = on;
    if( on ) setRunning( on );
    m_button->setVisible( !on );
}

void ClockBase::setFreq( double freq )
{
    if( freq <= 0 ) return;

    m_fstepsPC = 1e6*1e6/freq;
    m_stepsPC  = m_fstepsPC;
    
    m_freq = freq;
    m_remainder = 0;

    setRunning( m_isRunning || m_alwaysOn  );
}

void ClockBase::setRunning( bool running )
{
    m_button->setChecked( running );

    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void ClockBase::setVoltage( double v )
{
    m_voltage = v;
    m_outPin->setOutHighV( v );
    m_changed = true;
}

void ClockBase::setLinkedValue( double v, int )
{
    setFreq( v );
}

void ClockBase::setHidden( bool hide, bool hidArea, bool hidLabel )
{
    Component::setHidden( hide, hidArea, hidLabel );
    if  ( hidArea ) m_area = QRectF(  0, 0, -4, -4 );  // Totally hidden
    else if( hide ) m_area = QRectF(-30,-6, 12, 12 );  // In Board
    else            m_area = QRectF(-14,-8, 22, 16 );  // Normal

    m_proxy->setFlag( QGraphicsItem::ItemStacksBehindParent, hide && !hidArea );
}

void ClockBase::onbuttonclicked() { setRunning( !m_isRunning ); }
