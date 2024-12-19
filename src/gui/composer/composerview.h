/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef COMPOSERVIEW_H
#define COMPOSERVIEW_H

#include <QGraphicsView>

#include "composer.h"

class ComposerView : public QGraphicsView
{
    public:
        ComposerView( QWidget* parent );
        ~ComposerView();

        void clear();

        void wheelEvent( QWheelEvent* event );

        void mousePressEvent( QMouseEvent* event );
        //void mouseMoveEvent( QMouseEvent *event );
        void mouseReleaseEvent( QMouseEvent* event );

        void dragEnterEvent( QDragEnterEvent* event );
        void dragMoveEvent( QDragMoveEvent* event );
        void dragLeaveEvent( QDragLeaveEvent* event );

        void overrideCursor( const QCursor &cursor );

 static ComposerView* self() { return m_pSelf; }

    public slots:
        //void slotPaste();
        void importCirc();
        void zoomToFit();
        void zoomSelected();
        void zoomOne();

    protected:
        void contextMenuEvent( QContextMenuEvent* event );

    private:
 static ComposerView*  m_pSelf;

        qreal m_scale;

        FuncBlock* m_enterItem;

        Composer m_scene;
};
#endif
