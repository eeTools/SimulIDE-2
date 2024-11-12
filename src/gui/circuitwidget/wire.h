/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef WIRE_H
#define WIRE_H

#include <QGraphicsItem>

#include "compbase.h"

class Node;
class Pin;

class Wire : public CompBase, public QGraphicsItem
{
    Q_INTERFACES(QGraphicsItem)

    public:
        Wire( QString type, QString id, Pin* startpin, Pin* endpin = NULL );
        ~Wire();

        enum { Type = UserType + 2 };
        int type() const override { return Type; }

        QRectF boundingRect() const override;

        void dummySetter( QString ) {;}

        QString startPinId();
        QString endPinId();

        Pin* startPin() { return m_startPin;}
        void setStartPin( Pin* pin ) { m_startPin = pin; }

        Pin* endPin() { return m_endPin; }
        void setEndPin( Pin* pin) { m_endPin = pin; }

        QString pListStr() { refreshPointList(); return m_pointList.join(","); }
        //QStringList pointList() { refreshPointList(); return m_pointList; }
        QList<QPoint> pointVector() { return m_pList; }

        void setPointListStr( QString pl );
        void setPointList( QStringList pl );
        void setPointVector( QList<QPoint> pv );

        void incActLine() { if( m_actLine < m_pList.size()-1 ) m_actLine += 1; }

        bool connectToWire( QPoint cutPoint );
        void updateConRoute( QPointF thisPoint );
        void updateConRoute( Pin* nod );
        void closeCon( Pin* endpin );

        void setIsBus( bool bus ) { m_isBus = bus; }
        bool isBus() { return m_isBus; }

        double getVoltage();
        
        void move( QPointF delta );

        void remove();

        bool m_freeLine;

        QPainterPath shape() const override;

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseMoveEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) override;
        void contextMenuEvent( QGraphicsSceneContextMenuEvent* event ) override;
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* ) override;

    private:
        void moveLine( QPoint delta );
        void refreshPointList();
        void splitCon( int index, Node* node );
        int  eventPointIndex( QPoint cutPoint );

        int m_actLine;
        int m_lastIndex;
        
        bool m_isBus;
        bool m_moving;

        Pin* m_startPin;
        Pin* m_endPin;

        QStringList m_pointList;
        QList<QPoint> m_pList;

        QPoint m_lastPoint;
        QPoint m_eventPoint;

        QPainterPath m_path;
};

#endif
