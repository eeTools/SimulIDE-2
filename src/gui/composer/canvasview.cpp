/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QSvgGenerator>
#include <QMimeData>
#include <QFileDialog>
#include <QSettings>
#include <QGuiApplication>
#include <QMouseEvent>

#include "canvasview.h"
#include "canvas.h"
//#include "circuitwidget.h"
#include "mainwindow.h"

#include "utils.h"

#define tr(str) QCoreApplication::translate("CanvasView",str)

CanvasView* CanvasView::m_pSelf = nullptr;

CanvasView::CanvasView( QWidget* parent )
          : QGraphicsView( parent )
{
    setObjectName( "CanvasView" );
    if( !m_pSelf ) m_pSelf = this;

    m_canvas = nullptr;
    m_scale = 1;

    //viewport()->setFixedSize( 3200, 2400 );
    bool scrollBars = MainWindow::self()->settings()->value( "Circuit/showScroll" ).toBool();
    if( scrollBars ){
        setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
        setVerticalScrollBarPolicy(   Qt::ScrollBarAlwaysOn );
    }else{
        setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        setVerticalScrollBarPolicy(   Qt::ScrollBarAlwaysOff );
    }
    //setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
    //setCacheMode( CacheBackground );
    //setRenderHint( QPainter::Antialiasing );
    setRenderHints( QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setTransformationAnchor( AnchorUnderMouse );
    setResizeAnchor( AnchorUnderMouse );
    setDragMode( RubberBandDrag );

    setAcceptDrops( true );

}
CanvasView::~CanvasView() { }

void CanvasView::clear()
{
    if( m_canvas ) delete m_canvas;
    m_canvas = new Canvas(-1600,-1200, 3200, 2400, this );
    setScene( m_canvas );

    resetTransform();
    m_scale = 1;
    centerOn( 0, 0 );
}

void CanvasView::wheelEvent( QWheelEvent* event )
{
    qreal scaleFactor = pow( 2.0, event->delta() / 700.0);
    scale( scaleFactor, scaleFactor );
    m_scale *= scaleFactor;
}

void CanvasView::mousePressEvent( QMouseEvent* event )
{
    if( event->button() == Qt::MidButton )
    {
        event->accept();
        setDragMode( QGraphicsView::ScrollHandDrag );

        QGraphicsView::mousePressEvent( event );
        if( event->isAccepted() ) return;

        event = new QMouseEvent( QEvent::MouseButtonPress, event->pos(),
                                 Qt::LeftButton, Qt::LeftButton, Qt::NoModifier );
    }
    QGraphicsView::mousePressEvent( event );
}

void CanvasView::mouseMoveEvent( QMouseEvent* event )
{
    QGraphicsView::mouseMoveEvent( event );
}

void CanvasView::mouseReleaseEvent( QMouseEvent* event )
{
    if( event->button() == Qt::MidButton )
    {
        event->accept();
        QMouseEvent eve( QEvent::MouseButtonRelease, event->pos(),
            Qt::LeftButton, Qt::LeftButton, Qt::NoModifier   );

        QGraphicsView::mouseReleaseEvent( &eve );
        setDragMode( QGraphicsView::RubberBandDrag );
    }
    else QGraphicsView::mouseReleaseEvent( event );
}

void CanvasView::zoomToFit()
{
    QRectF r = m_canvas->itemsBoundingRect();
    fitInView( r, Qt::KeepAspectRatio );
}


void CanvasView::overrideCursor( const QCursor &cursor )
{
    setDragMode( QGraphicsView::ScrollHandDrag );
    setCursor( cursor );
    setDragMode( QGraphicsView::RubberBandDrag );
}

void CanvasView::contextMenuEvent( QContextMenuEvent* event )
{
    QGraphicsView::contextMenuEvent( event );
}
