/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QGraphicsView>

class Canvas;

class CanvasView : public QGraphicsView
{
    public:
        CanvasView( QWidget* parent );
        ~CanvasView();

 static CanvasView* self() { return m_pSelf; }
 
        void clear();
        
        void wheelEvent( QWheelEvent* event );

        void mousePressEvent( QMouseEvent* event );
        void mouseMoveEvent( QMouseEvent *event );
        void mouseReleaseEvent( QMouseEvent* event );

        void zoomToFit();

        void overrideCursor( const QCursor &cursor );
        
    protected:
        void contextMenuEvent( QContextMenuEvent* event );

    private:
 static CanvasView*  m_pSelf;

        qreal m_scale;

        Canvas* m_canvas;
};

#endif
