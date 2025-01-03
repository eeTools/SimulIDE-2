/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "diode.h"
//#include "simulator.h"
#include "pin.h"

#include "doubleprop.h"
#include "stringprop.h"

#define tr(str) simulideTr("Diode",str)

listItem_t Diode::registerItem(){
    return {
        tr("Diode"),
        "Rectifiers",
        "diode.png",
        "Diode",
        [](QString id){ return (CompBase*)new Diode( id ); } };
}

Diode::Diode( QString id, bool )
     : Component( id )
     , PN( id )
{
    m_area = QRect(-10, -8, 20, 16 );

    m_pin << new Pin( 180, QPoint(-16, 0 ), "-nPin@"+id, this );
    m_pin << new Pin( 0,   QPoint( 16, 0 ), "-nPin@"+id, this );
}
Diode::~Diode(){}

void Diode::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    p->setBrush( Qt::black );

 static const QPointF points[3] = {
        QPointF( 7, 0 ),
        QPointF(-8,-7 ),
        QPointF(-8, 7 )           };
    p->drawPolygon(points, 3);

    QPen pen = p->pen();
    pen.setWidth( 3 );
    p->setPen( pen );
    p->drawLine( 7, -6, 7, 6 );

    Component::paintSelected( p );
}
