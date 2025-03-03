/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef COMPLABEL_H
#define COMPLABEL_H

#include <QGraphicsTextItem>

class Component;

class Label : public QGraphicsTextItem
{
    friend class Component;

    public:
        Label( Component* parent );
        ~Label();

        QPointF getLabelPos() { return QPointF( m_labelx, m_labely ); }
        void setLabelPos( QPointF pos );
        void updtLabelPos();

        int  getAngle()            { return m_labelrot; }
        void setAngle( int angle ) { m_labelrot = angle;  }
        void rotateAngle( double a );

        void setLabelText( QString text );
        void setLine( uint8_t prop, QString newLine );

        void showLabel();

    public slots:
        void rotateCW();
        void rotateCCW();
        void rotate180();
        void updateGeometry(int, int, int);

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent* event );
        void mouseMoveEvent( QGraphicsSceneMouseEvent* event );
        void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );
        void contextMenuEvent( QGraphicsSceneContextMenuEvent* event );
        int hFlip();
        int vFlip();

    private:
        QPointF adjustPos( float x, float y, int width, int height );
        void updateLines();

        QString m_text;

        Component* m_parentComp;

        float m_labelx;
        float m_labely;
        int m_labelrot;

        QMap<uint8_t, QString> m_lines;
};
#endif
