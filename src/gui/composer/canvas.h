/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsScene>

class Canvas : public QGraphicsScene
{
    Q_OBJECT

    friend class ComposerWidget;

    public:
        Canvas( qreal x, qreal y, qreal w, qreal h, QGraphicsView* parent );
        ~Canvas();

 static Canvas* self() { return m_pSelf; }

        virtual void drawBackground( QPainter* painter, const QRectF &rect ) override;

    private:
 static Canvas* m_pSelf;

        QRect m_scenerect;
};

#endif
