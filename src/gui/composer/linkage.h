/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef LINKAGE_H
#define LINKAGE_H

#include <QGraphicsItem>

#include "route.h"

class Node;

class Linkage : public Route, public QGraphicsItem
{
    Q_INTERFACES(QGraphicsItem)

    public:
        Linkage( QString id, PinBase* startpin, PinBase* endpin=nullptr );
        ~Linkage();

        int type() const override { return UserType + 2; } // QGraphicsItem type

        QRectF boundingRect() const override;

        QList<QPoint> pointVector() { return m_pList; }
        void setPointVector( QList<QPoint> pv );

        void setPointListStr( QString pl ) override;
        void setPointList( QStringList pl ) override;

        void updateConRoute( QPointF thisPoint ) override;
        void updateConRoute( PinBase* nod ) override;
        void closeCon( PinBase* endpin ) override;

        void remove() override;

        void move( QPointF delta ) override;

        void setVisib( bool v ) override { setVisible( v ); }
        void select( bool s )  override { setSelected( s ); }

        QPainterPath shape() const override;

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseMoveEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) override;
        void contextMenuEvent( QGraphicsSceneContextMenuEvent* event ) override;
        void paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* ) override;

        bool connectToWire( QPoint cutPoint );
        void moveLine( QPoint delta );
        void refreshPointList() override;
        void splitCon( int index, Node* node );
        int  eventPointIndex( QPoint cutPoint );

        bool m_moving;

        QPoint m_lastPoint;
        QPoint m_eventPoint;

        QPainterPath m_path;
};

#endif
