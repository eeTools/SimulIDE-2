/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>

#include <QWheelEvent>
#include <QMimeData>
#include <QDebug>

#include "composerview.h"
#include "composer.h"
#include "fblock.h"
#include "module.h"

ComposerView* ComposerView::m_pSelf = nullptr;

ComposerView::ComposerView( QWidget* parent )
            : QGraphicsView( parent )
            , m_scene( 3200, 2400, this )
{
    m_pSelf = this;
    m_scale = 1;

    bool scrollBars = false;//MainWindow::self()->settings()->value( "Circuit/showScroll" ).toBool();
    if( scrollBars ){
        setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
        setVerticalScrollBarPolicy(   Qt::ScrollBarAlwaysOn );
    }else{
        setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        setVerticalScrollBarPolicy(   Qt::ScrollBarAlwaysOff );
    }
    setRenderHints( QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setTransformationAnchor( AnchorUnderMouse );
    setResizeAnchor( AnchorUnderMouse );
    setDragMode( RubberBandDrag );
    setAcceptDrops(true);
    setScene( &m_scene );

    resetTransform();

    scale( 1.2, 1.2 );
    m_scale = 1.2;

    centerOn( 0, 0 );
}
ComposerView::~ComposerView(){}

void ComposerView::clear()
{
    m_scene.clearCanvas();

    resetTransform();
    m_scale = 1;
    m_enterItem = nullptr;
    centerOn( 0, 0 );
}

void ComposerView::wheelEvent( QWheelEvent* event )
{
    qreal scaleFactor = pow( 2.0, event->delta() / 700.0);
    scale( scaleFactor, scaleFactor );
    m_scale *= scaleFactor;
}

void ComposerView::mousePressEvent( QMouseEvent* event )
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

/*void ComposerView::mouseMoveEvent( QMouseEvent* event )
{
    QGraphicsView::mouseMoveEvent( event );
}*/

void ComposerView::mouseReleaseEvent( QMouseEvent* event )
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

void ComposerView::dragEnterEvent( QDragEnterEvent* event )
{
    event->accept();
    m_enterItem = nullptr;

    QString type = event->mimeData()->html();
    QString name = event->mimeData()->text();
//qDebug() <<"ComposerView::dragEnterEvent" << type << name;
    //if( type.isEmpty() || name.isEmpty() ) return;

    m_enterItem = Composer::self()->createBlock( nullptr, type );
    if( m_enterItem )
    {
        //Composer::self()->clearSelection();
        //Composer::self()->addItem( m_enterItem );
        //Composer::self()->compList()->insert( m_enterItem );
        //Composer::self()->saveCompChange( m_enterItem->getUid(), COMP_STATE_NEW, "" );
//qDebug() <<"ComposerView::dragEnterEvent" << type << name;
        m_enterItem->setPos( mapToScene( event->pos() ) );
        m_enterItem->module()->setup();
        //m_enterItem->setSelected( true );

        this->setFocus();
    }
}

void ComposerView::dragMoveEvent( QDragMoveEvent* event )
{
    event->accept();
    if( m_enterItem ) m_enterItem->setPos( mapToScene( event->pos() ) );
}

void ComposerView::dragLeaveEvent( QDragLeaveEvent* event )
{
    event->accept();
    if( !m_enterItem ) return;

    Composer::self()->removeBlock( m_enterItem );
    //Circuit::self()->removeLastUndo();
    m_enterItem = nullptr;
}

void ComposerView::overrideCursor( const QCursor &cursor )
{
    setDragMode( QGraphicsView::ScrollHandDrag );
    setCursor( cursor );
    setDragMode( QGraphicsView::RubberBandDrag );
}

void ComposerView::zoomToFit()
{
    QRectF r = m_scene.itemsBoundingRect();
    fitInView( r, Qt::KeepAspectRatio );
}

void ComposerView::zoomSelected()
{
    QRectF r;
    for( QGraphicsItem *item : m_scene.items() )
      if( item->isSelected() ) r |= item->sceneBoundingRect();

    fitInView( r, Qt::KeepAspectRatio );
}

void ComposerView::zoomOne()
{
    resetTransform();
    m_scale = 1;
}

void ComposerView::contextMenuEvent( QContextMenuEvent* event )
{

    QGraphicsView::contextMenuEvent( event );

    if( m_scene.getNewWire() ) m_scene.deleteNewWire();
}
