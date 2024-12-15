/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QGraphicsView>

#include "canvas.h"

Canvas* Canvas::m_pSelf = nullptr;

Canvas::Canvas( qreal x, qreal y, qreal w, qreal h, QGraphicsView* parent )
      : QGraphicsScene( x, y, w, h, parent )
{
    m_pSelf = this;

    setObjectName("Canvas");

    m_scenerect.setRect( x, y, w, h );
    setSceneRect( QRectF(x, y, w, h) );
}
Canvas::~Canvas(){}

void Canvas::drawBackground( QPainter* painter, const QRectF &rect )
{
    /*painter->setBrush(QColor( 255, 255, 255 ) );
    painter->drawRect( m_scenerect );
    return;*/

    painter->setBrush( QColor( 240, 240, 210 ) );
    painter->drawRect( m_scenerect );
    painter->setPen( QColor( 210, 210, 210 ) );

    //if( m_hideGrid ) return;

    int startx = int(m_scenerect.x());///2;
    int endx   = int(m_scenerect.width())/2;
    int starty = int(m_scenerect.y());///2;
    int endy   = int(m_scenerect.height())/2;

    for( int i=4; i<endx; i+=8 ){
        painter->drawLine( i, starty, i, endy );
        painter->drawLine(-i, starty,-i, endy );
    }
    for( int i=4; i<endy; i+=8 ){
        painter->drawLine( startx, i, endx, i);
        painter->drawLine( startx,-i, endx,-i);
}   }
