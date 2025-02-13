/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "ground.h"
#include "iopin.h"
#include "pin.h"

#define tr(str) simulideTr("Ground",str)

listItem_t Ground::registerItem(){
    return {
        tr("Ground (0 V)"),
        "Sources",
        "ground.png",
        "Ground",
        [](int id){ return (CompBase*)new Ground( id ); } };
};

Ground::Ground( int id )
      : Component( id )
{
    m_area = QRect(-6,-1, 12, 9 );

    m_out = new IoPin( 90, QPoint( 0,-8 ), "Pin@"+id, this );
    m_pin << m_out;
    m_out->setOutputImp( 1e-9 );

    setLabelPos(-16, 8, 0);
}
Ground::~Ground() {}

void Ground::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    QPen pen = p->pen();
    pen.setWidthF( 2.5 );
    p->setPen(pen);

    p->drawLine(QLineF(-6.6, 0, 6.6,-0) );
    p->drawLine(QLineF(-4.3, 4, 4.3, 4) );
    p->drawLine(QLineF(-1.9, 8, 1.9, 8) );

    Component::paintSelected( p );
}
