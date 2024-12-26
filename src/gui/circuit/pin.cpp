/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>

#include <QApplication>
#include <QPainter>

#include "pin.h"
#include "node.h"
#include "lachannel.h"
#include "circuit.h"
#include "wire.h"
#include "wireline.h"
#include "component.h"
#include "simulator.h"

Pin::Pin( int angle, QPoint pos, QString id, Component* parent, int length )
   : PinBase( angle, pos, parent, length )
   , ePin( id )
{
    m_area = QRect(-3, -3, 11, 6);

    m_component = parent;
    m_pinState  = undef_state;
    m_pinType   = pinNormal;

    m_conPin = nullptr;
    m_dataChannel = nullptr;
    
    m_color[0] = Qt::black;
    m_color[1] = QColor( 100, 100, 250 );
    m_color[2] = QColor( 0, 0, 180 );;
    m_color[3] = QColor( 60, 120, 60 );
    m_color[4] = QColor( 0, 0, 180 );
    m_color[5] = QColor( 180, 0, 0 );
    m_color[6] = QColor( 100, 100, 250 );
    m_color[7] = QColor( 250, 120, 0 );

    Circuit::self()->addPin( this, id );
    Pin::animate( Circuit::self()->animate() );

    setFlag( QGraphicsItem::ItemStacksBehindParent, true );

    m_component->addSignalPin( this );
}
Pin::~Pin()
{
    Circuit::self()->remPin( m_id );
}

void Pin::updateStep()
{
    if( m_unused ) return;
    //if( m_PinChanged )
        update();
}

void Pin::removeWire()
{
    if( m_wire ) Circuit::self()->removeWire( (Wire*)m_wire );
    PinBase::removeWire();
}

void Pin::wireRemoved( WireBase* w )
{
    setWire( nullptr );
    if( !Circuit::self()->undoRedo() ) m_component->pinMessage( 1 ); // Used by node to remove
    /// m_component->remSignalPin( this ); after conn removed it can't auto-connect again
}

void Pin::registerPinsW( int enode, int n )     // Called by component, calls conPin
{
    if( m_blocked ) return;
    m_blocked = true;

    if( !(m_wireFlags & wireBus) ){
        m_enode = enode;
        Simulator::self()->addToPinList( m_id );
    }
    if( m_conPin ) m_conPin->registerEnode( enode, n ); // Call pin at other side of Connector

    m_blocked = false;
}

void Pin::registerEnode( int enode, int n )     // Called by m_conPin
{
    if( m_blocked ) return;
    m_blocked = true;

    if( !(m_wireFlags & wireBus) ){
        m_enode = enode;
        Simulator::self()->addToPinList( m_id );
    }
    m_component->registerEnode( enode, n );
    if( m_dataChannel ) m_dataChannel->registerEnode( enode, n );

    m_blocked = false;
}

Pin* Pin::connectPin( bool connect )      // Auto-Connect
{
    Pin* _pin = nullptr;
    QList<QGraphicsItem*> list = this->collidingItems();
    while( !list.isEmpty() )
    {
        QGraphicsItem* it = list.takeLast();
        if( it->type() == 65536+3 )          // Pin found
        {
            Pin* pin =  qgraphicsitem_cast<Pin*>( it );

            if( pin->parentItem() == this->parentItem() ) continue;
            if( fabs(scenePos().x()-pin->scenePos().x()) > 3 ) continue;
            if( fabs(scenePos().y()-pin->scenePos().y()) > 3 ) continue;
            if( m_wireFlags != pin->wireFlags() ) continue; // Only connect Bus to Bus
            if( pin->wire() ) continue;
            if( pin->unused() ) continue;
            if( !connect && pin->pinType() < pinSocket
              && pin->isVisible() && !pin->isObscuredBy( m_component ) ) _pin = pin;
            if( connect )
            {
                Circuit::self()->startWire( this, true );
                Circuit::self()->closeWire( pin, true );
            }
            break;
        }
        else if( connect && (it->type() == UserType+2) ) // WireLine
        {
            WireLine* line =  qgraphicsitem_cast<WireLine*>( it );
            if( m_wireFlags != line->connector()->wireFlags() ) continue;
            Circuit::self()->startWire( this );
            line->connectToWire( QPoint( scenePos().x(), scenePos().y()) );
            break;
        }
    }
    return _pin;
}

void Pin::isMoved()
{
    if( m_wire ) m_wire->updateConRoute( this );
    else if( this->isVisible() && !m_wire )
    {                                       // Auto-Connect
        if( !Circuit::self()->isBusy()
         && (QApplication::queryKeyboardModifiers() == Qt::ShiftModifier) )
            connectPin( true );
    }
    setLabelPos();
}

void Pin::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    if( m_unused ) return;
    
    if( event->button() == Qt::LeftButton ) // Start/Close Wire
    {
        if( m_wire ) event->ignore();
        else{
            WireBase* wire = Circuit::self()->getNewWire();
            if( wire )
            {
                if( m_wireFlags != wire->wireFlags()  ) // Avoid connect Bus with no-Bus
                { event->ignore(); return; }
            }
            event->accept();
            if( wire ) Circuit::self()->closeWire( this, true );
            else       Circuit::self()->startWire( this, true );
}   }   }

void Pin::animate( bool an )
{
    if( m_wireFlags & wireBus ) return;
    PinBase::animate( an );
}

void Pin::paint( QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    if( !isVisible() ) return;
    //m_PinChanged = false;

    /*QPen pen0( m_color[0], 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    painter->setPen(pen0);
    painter->setBrush( Qt::red );
    painter->drawRect( boundingRect() );*/

    if( m_warning )
    {
        static double opaci = 0.3;
        static double speed = 0.05;
        if( Simulator::self()->isRunning() ) m_opCount += speed;
        else                                 m_opCount = 0.65;

        painter->setOpacity( m_opCount+opaci );
        if( m_opCount > 0.7 ) m_opCount = 0.0;

        painter->fillRect( QRect(-4, -4, m_length+4, 8 ), QColor(200, 100, 0, 240) );
    }

    if( m_overScore > -1 )
    {
        qreal x = m_length+2.5;
        qreal y = -m_labelheight+1;
        qreal width = m_labelWidth-1;
        if( m_angle == 0 ) y = -y; // Right

        QPen pen( m_label.brush().color(), 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
        painter->setPen( pen );
        painter->drawLine( QPointF( x, y ), QPointF( x+width, y ) );
    }
    QPen pen( m_color[0], 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );

    if     ( m_unused  ) pen.setColor( QColor( 75, 120, 170 ));
    else if( m_wireFlags & wireBus ) pen.setColor( Qt::darkGreen );
    else if( m_animate ) pen.setColor( m_color[m_pinState] );

    painter->setPen(pen);
    if( m_length > 1 ) painter->drawLine( 0, 0, m_length-1, 0);
    else               painter->drawLine( QPointF(-0.01, 0 ), QPointF( 0.03, 0 ));

    if( m_inverted )
    {
        painter->setBrush( Qt::white );
        QPen pen = painter->pen();
        pen.setWidthF( 1.8 );
        painter->setPen(pen);
        QRectF rect( 3.5,-2.2, 4.4, 4.4 );
        painter->drawEllipse(rect);
    }
    if( !m_unused && m_animate )
    {
        pen.setWidthF( 1.5 );
        painter->setPen(pen);
        if( m_pinState >= input_low ) // Draw Input arrow
        {
            painter->drawLine( 2, 0, 0, 2);
            painter->drawLine( 0,-2, 2, 0);
        }else{
            if( m_pinState >= out_low ) // Draw lower half Output arrow
            painter->drawLine( 0, 0, 2, 2);
            if( m_pinState >= driven_low )
            painter->drawLine( 2,-2, 0, 0);// Draw upper half Output arrow
        }
}   }
