/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "mosfet.h"
#include "pin.h"
#include "simulator.h"
#include "circuit.h"

//#include "doubleprop.h"
//#include "boolprop.h"

#define tr(str) simulideTr("Mosfet",str)

listItem_t Mosfet::registerItem(){
    return {
        tr("Mosfet"),
        "Transistors",
        "mosfet.png",
        "Mosfet",
        [](QString id){ return (CompBase*)new Mosfet( id ); } };
}

Mosfet::Mosfet( QString id )
      : Component( id )
      , Fet( id )
{
    m_area = QRectF(-12, -14, 28, 28 );
    setLabelPos( 18, 0, 0 );

    m_pin << new Pin(  90, QPoint(  8,-16), "PinD@"+id, this );
    m_pin << new Pin( 270, QPoint(  8, 16), "PinS@"+id, this );
    m_pin << new Pin( 180, QPoint(-16, 0 ), "PinG@"+id, this );
}
Mosfet::~Mosfet(){}


void Mosfet::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );
    
    p->drawEllipse( m_area );
    
    p->drawLine(-12, 0,-4, 0 );
    p->drawLine( -4,-8,-4, 8 );
    
    p->drawLine( 0,-7, 8,-7 );
    p->drawLine( 0, 0, 8, 0 );
    p->drawLine( 0, 7, 8, 7 );
    
    p->drawLine( 8,-12, 8,-7 );
    p->drawLine( 8, 12, 8, 0 );
    
    p->setBrush( Qt::black );
    if( m_Pchannel )
    {
         QPointF points[3] = {
         QPointF( 7, 0 ),
         QPointF( 3,-2 ),
         QPointF( 3, 2 ) };
         p->drawPolygon( points, 3 );
    } else {
        QPointF points[3] = {
        QPointF( 1, 0 ),
        QPointF( 5,-2 ),
        QPointF( 5, 2 )     };
        p->drawPolygon( points, 3 );
    }
    if( m_depletion )
    {
        QPen pen(Qt::black, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p->setPen( pen );
        p->drawLine( 0,-9, 0, 9 );
    } else {
        p->drawLine( 0,-9, 0,-5 );
        p->drawLine( 0,-2, 0, 2 );
        p->drawLine( 0, 5, 0, 9 );
    }
    Component::paintSelected( p );
}
