/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>

//#include <QDebug>

#include <QApplication>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "hook.h"
#include "funcwire.h"
#include "composer.h"

Hook::Hook( int angle, QPoint pos, QString id, int index, hookType_t type, QGraphicsItem* parent )
    : PinBase( angle, pos, id, parent )
{
    m_id = id;
    m_hookType = type;
    m_pinType = pinHook;
    m_wireFlags = wireFunc;

    m_output = (angle == 0);
    m_angle = angle;
    m_space = 0;

    m_area = QRect(-4,-4, 8, 8);

    m_color[0] = QColor(  50,  50,  50 ); // None
    m_color[1] = QColor( 250, 240, 240 ); // Property
    m_color[2] = QColor(  50, 150, 255 ); // Input bit
    m_color[3] = QColor(  50, 255, 100 ); // Input int
    m_color[4] = QColor( 255, 200,  30 ); // Input doub
    m_color[5] = QColor(  50, 150, 255 ); // Output bit
    m_color[6] = QColor(  50, 255, 100 ); // Output int
    m_color[7] = QColor( 255, 200,  30 ); // Output doub

    setFontSize( 11 );
    setAcceptHoverEvents( true );

    m_label.setBrush( m_color[type] );

    setFlag( QGraphicsItem::ItemStacksBehindParent, false );

    //Circuit::self()->addHook( this, id );

}
Hook::~Hook(){}

void Hook::setWire( WireBase* w )
{
    if( m_wireList.contains(w) ) return;
    m_wireList.append( w );

    if( m_wireList.isEmpty() ) setCursor( Qt::CrossCursor );
    else if( !m_output )       setCursor( Qt::ArrowCursor );
}

void Hook::wireRemoved( WireBase* w )
{
    if( m_wireList.contains(w) ) m_wireList.removeOne( w );
}

void Hook::isMoved()
{
    for( WireBase* wire : m_wireList ) wire->updateConRoute( this );
    setLabelPos();
}

void Hook::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    if( event->button() == Qt::LeftButton ) // Start/Close Connector
    {
        event->accept();

        if( Composer::self()->getNewWire() ){
            if( !m_output && m_wireList.isEmpty() )
            {
                Composer::self()->closeWire( this );
            }
        }
        else if( m_output ) Composer::self()->startWire( this );
    }
}

void Hook::paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* )
{
    if( !isVisible() ) return;

    /*QPen pen0( m_color[0], 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen(pen0);
    p->setBrush( Qt::red );
    p->drawRect( boundingRect() );*/

    QPen pen( Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen );

    p->setBrush( m_color[m_hookType] );

    if( m_angle == 0 )         // Left side: Slots
    {
        static const QPointF points[4] = {
            QPointF(-2, 0.5 ),
            QPointF(-2,-0.5 ),
            QPointF( 4,-4 ),
            QPointF( 4, 4 )            };

            p->drawPolygon( points, 4 );
    }
    else                      // Right side: Signals
    {
        if( m_hookType > hookProperty )
        {
        static const QPointF points[4] = {
            QPointF( 4, 0.5 ),
            QPointF( 4,-0.5 ),
            QPointF(-2,-4 ),
            QPointF(-2, 4 )            };

            p->drawPolygon( points, 4 );
        }
        else
        {
            p->drawEllipse( QRect(-3,-3, 6, 6 ) );
        }
    }
    //p->drawLine( QPointF(-0.01, 0 ), QPointF( 0.03, 0 ));
}
