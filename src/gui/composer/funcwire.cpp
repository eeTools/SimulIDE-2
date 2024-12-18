/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>

#include "funcwire.h"
#include "circuitwidget.h"
#include "simulator.h"
#include "composer.h"
#include "node.h"
#include "pin.h"
#include "utils.h"

#define tr(str) simulideTr("FuncWire",str)

FuncWire::FuncWire( QString id, PinBase* startpin, PinBase* endpin )
       : WireBase( id, startpin )
       , QGraphicsItem()
{
    m_type = "FuncWire";

    m_moving = false;

    m_wireFlags = m_startPin->wireFlags();

    QPoint pinPos = startPin()->scenePos().toPoint();
    m_pList.append( pinPos );
    m_lastPoint = pinPos;

    if( endpin ) closeCon( endpin );
    else m_endPin = nullptr;

    Composer::self()->compMap()->insert( id, this );
    Composer::self()->addItem( this );
}
FuncWire::~FuncWire()
{
    Composer::self()->compMap()->remove( m_id );
}

QRectF FuncWire::boundingRect() const
{
    return m_path.controlPointRect() + QMarginsF( 2, 2, 2, 2 );
}

void FuncWire::setPointListStr( QString pl )
{
    setPointList( pl.split(",") );
}

void FuncWire::setPointList( QStringList plist )
{
    m_pointList = plist;

    m_pList.clear();
    for( int i=0; i<plist.size(); i+=2 )
    {
        QPoint p1( plist.at(i).toInt(), plist.at(i+1).toInt() );
        m_pList.append( p1 );
    }
    m_lastPoint = m_pList.takeLast();
}

void FuncWire::setPointVector( QList<QPoint> pv)
{
    m_pList = pv;
    m_lastPoint = m_pList.takeLast();
}

void FuncWire::refreshPointList()
{
    m_pointList.clear();
    for( QPoint point : m_pList )
    {
        m_pointList.append( QString::number( point.x() ) );
        m_pointList.append( QString::number( point.y() ) );
    }
    m_pointList.append( QString::number( m_lastPoint.x() ) );
    m_pointList.append( QString::number( m_lastPoint.y() ) );
}

void FuncWire::updateConRoute( QPointF thisPoint )
{
    QPoint p0 = m_pList.last();
    QPoint newPoint = toGrid( thisPoint ).toPoint();

    if( newPoint == m_lastPoint ) return;

    int index = m_pList.size()-1;

    QPoint delta0 = m_lastPoint - p0;
    QPoint delta1 = newPoint - m_lastPoint;

    if( m_actLine == index )
    {
        if( delta0.y() == 0 ) // Last line Horizontal
        {
            m_lastPoint.setX( newPoint.x() );

            if( delta1.y() ) // create corner
            {
                m_pList.append( m_lastPoint );
                m_lastPoint.setY( newPoint.y() );
            }
        }
        else if( delta0.x() == 0 ) // Last line Vertical
        {
            m_lastPoint.setY( newPoint.y() );

            if( delta1.x() ) // create corner
            {
                m_pList.append( m_lastPoint );
                m_lastPoint.setX( newPoint.x() );
            }
        }
    }else{
        m_lastPoint = newPoint;

        if( delta0.y() == 0 ) // Last line Horizontal
        {
            if( newPoint.x() == p0.x()  && m_pList.size() > 1 ) // 2 Vertical lines
            {
                m_pList.takeLast(); // Remove last
                if( m_actLine > 0 ) m_actLine--;
            }
            else if( delta1.y() )
            {
                p0.setY( newPoint.y() );
                m_pList.replace( index, p0 );
            }
        }
        else if( delta0.x() == 0 ) // Last line Vertical
        {
            if( newPoint.y() == p0.y()  && m_pList.size() > 1 ) // 2 Horizontal lines
            {
                m_pList.takeLast(); // Remove last
                if( m_actLine > 0 ) m_actLine--;
            }
            else if( delta1.x() )
            {
                p0.setX( newPoint.x() );
                m_pList.replace( index, p0 );
            }
        }
    }
    Composer::self()->update();
}

void FuncWire::updateConRoute( PinBase* pin )
{
    if( pin == m_startPin ) // Convert startPin in endPin
    {
        m_startPin = m_endPin;
        m_endPin = pin;
        QList<QPoint> list;

        for( QPoint point : m_pList ) list.prepend( point );
        list.prepend( m_lastPoint );

        m_lastPoint = list.takeLast();
        m_pList = list;
    }
    updateConRoute( pin->scenePos() );
}

void FuncWire::move( QPointF delta )
{
    if( !Composer::self()->pasting() ) return;

    setPos( pos() + delta );
    //m_startPin->isMoved();
    //m_endPin->isMoved();
}

void FuncWire::remove()
{
    if( Simulator::self()->isRunning() )  CircuitWidget::self()->powerCircOff();

    if( m_startPin ) m_startPin->wireRemoved();
    if( m_endPin )   m_endPin->wireRemoved();

    Composer::self()->removeItem( this );
}

void FuncWire::closeCon( PinBase* endpin )
{
    m_endPin = endpin;
    m_startPin->setWire( this );
    m_endPin->setWire( this );
    m_startPin->setConPin( m_endPin );
    m_startPin->setWireFlags( m_wireFlags ); // StartPin will set conPin flags
    m_endPin->setConPin( m_startPin );
    m_startPin->setWireFlags( m_wireFlags );

    updateConRoute( m_endPin->scenePos() );

    setCursor( Qt::CrossCursor );
}

void FuncWire::splitCon( int index, Node* node )
{
    if( !m_endPin ) return;

    QPoint cutPoint = node->scenePos().toPoint();

    FuncWire* wire0 = new FuncWire( Composer::self()->newWireId(), m_startPin );
    Composer::self()->wireList()->append( wire0 );

    QList<QPoint> poinVector;

    for( int i=0; i<index; ++i ) poinVector.append( m_pList.takeFirst() );
    poinVector.append( cutPoint );
    wire0->setPointVector( poinVector );
    wire0->closeCon( node->getPin(0) );

    m_pList.prepend( cutPoint );
    m_startPin = node->getPin(2);
    closeCon( m_endPin );
}

int FuncWire::eventPointIndex( QPoint cutPoint )
{
    QPoint point0;
    QPoint point1;
    int i=0;
    for( i=0; i<m_pList.size(); i++ ) // Find point to split
    {
        point0 = m_pList.at( i );
        QPoint delta0 = cutPoint - point0;
        if( delta0 == QPoint( 0, 0 ) ) break; // Point at corner

        point1 = (i < m_pList.size()-1 ) ? m_pList.at( i+1 ) : m_lastPoint;
        QPoint delta1 = cutPoint - point1;
        if( delta1 == QPoint( 0, 0 ) ) break; // Point at corner

        if( delta0.x() == 0 ) // Vertical line
        {
            if( (delta0.y() > 0) != (delta1.y() > 0) ) break;
        }
        if( delta0.y() == 0 ) // Horizontal line
        {
            if( (delta0.x() > 0) != (delta1.x() > 0) ) break;
        }
    }
    return i;
}

bool FuncWire::connectToWire( QPoint cutPoint )
{
    int index = eventPointIndex( cutPoint )+1;

    if( Simulator::self()->isRunning() )  CircuitWidget::self()->powerCircOff();

    if( !Composer::self()->getNewWire() ) Composer::self()->beginUndoStep(); // A new FuncWire started here

    Node* node = new Node( Composer::self()->newSceneId() );     // Now add the Node
    node->setPos( cutPoint );
    Composer::self()->addNode( node );

    PinBase* pin1 = node->getPin(1);
    if( Composer::self()->getNewWire() )   // A FuncWire wants to connect here (ends in a node)
    {
        splitCon( index, node );
        Composer::self()->closeWire( pin1, true );
    }
    else                                     // A new FuncWire created here (starts in a node)
    {
        pin1->setWireFlags( m_wireFlags );
        Composer::self()->startWire( pin1, false );
        splitCon( index, node );
    }
    return true;
}

void FuncWire::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    if( event->button() == Qt::MidButton )    // Move Line
    {
        event->accept();

        m_eventPoint = toGrid( event->scenePos().toPoint());
        m_lastIndex = eventPointIndex( m_eventPoint );

        ///if     ( dy() == 0 ) CircuitView::self()->viewport()->setCursor( Qt::SplitVCursor );
        ///else if( dx() == 0 ) CircuitView::self()->viewport()->setCursor( Qt::SplitHCursor );
        ///else                 CircuitView::self()->viewport()->setCursor( Qt::SizeAllCursor );
    }
    else if( event->button() == Qt::LeftButton )
    {
        if( event->modifiers() == Qt::ControlModifier ) setSelected( !isSelected() ); // Select - Deselect

        else if( event->modifiers() & Qt::ShiftModifier ) // Move Corner
        {
            QPoint evPoint = toGrid( event->scenePos() ).toPoint();

            ///if     ( evPoint==p1() ) m_moveP1 = true;
            ///else if( evPoint==p2() ) m_moveP2 = true;
        }
        else                                          // Connecting a wire here
        {
            WireBase* route = Composer::self()->getNewWire();
            if( Composer::self()->getNewWire() )     // FuncWire started at Pin is connecting here
            {
                FuncWire* wire = static_cast<FuncWire*>(route);
                if( wire == this ) return;
                if( wire->wireFlags() != m_wireFlags ) { event->ignore(); return; } // Avoid connect Bus with no-Bus
            }
            QPoint point1 = toGrid( event->scenePos() ).toPoint();

            if( connectToWire( point1 ) ) event->accept();
            else                          event->ignore();
}   }   }

void FuncWire::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
    event->accept();

    if( !m_moving && !Composer::self()->getNewWire() )
    {
        Composer::self()->saveCompChange( getUid(), "pointList", pListStr() );
        m_moving = true;
    }
    if( !m_moving ) return;

    QPoint eventPoint = toGrid( event->scenePos() ).toPoint();

    if( event->modifiers() & Qt::ShiftModifier )          // Move Corner
    {
        ///if     ( m_moveP1 ) setP1( p1()+delta );
        ///else if( m_moveP2 ) setP2( p2()+delta );
    }
    else
    {
        if( m_lastIndex == 0 )  // First line
        {
            m_pList.prepend( m_pList.first() );
            m_lastIndex = 1;
        }
        else if( m_lastIndex == m_pList.size()-1 ) // Last line
        {
            m_pList.append( m_lastPoint );
        }
        moveLine( eventPoint - m_eventPoint );
        m_eventPoint = eventPoint;
    }
}

void FuncWire::moveLine( QPoint delta )
{
    QPoint point0 = m_pList.at( m_lastIndex );
    QPoint point1 = m_pList.at( m_lastIndex+1 );

    QPoint mov = QPoint( 0, 0 );
    QPoint dP = point1 - point0;
    if     ( dP.x() == 0 ) mov = QPoint( delta.x(), 0 ); // Vertical
    else if( dP.y() == 0 ) mov = QPoint( 0, delta.y() ); // Horizontal

    m_pList.replace( m_lastIndex  , point0+mov );
    m_pList.replace( m_lastIndex+1, point1+mov );
    Composer::self()->update();
}

void FuncWire::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
    event->accept();
    m_moving = false;
}

void FuncWire::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
    if( !m_endPin
     || Composer::self()->getNewWire() ) return;

   event->accept();
   QMenu menu;

   QAction* removeAction = menu.addAction( tr("Remove") );
   QObject::connect( removeAction, &QAction::triggered
                   , [=](){ Composer::self()->removeWire( this ); } );

   /// TODO: set FuncWire color

   menu.exec( event->screenPos() );
}

QPainterPath FuncWire::shape() const
{
    QPainterPath path;

    QPainterPathStroker qp;
    qp.setWidth( 3.8 );
    qp.setCapStyle( Qt::FlatCap );

    return qp.createStroke( m_path );
}

void FuncWire::paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* )
{
    if( m_pList.isEmpty() ) return;
    m_path.clear();

    QPoint p0 = m_pList.first();
    m_path.moveTo( p0 );

    for( int i=1; i<m_pList.size(); ++i )
    {
        QPoint p1 = m_pList.at(i);

        QPoint p2 = (i == m_pList.size()-1) ? m_lastPoint : m_pList.at(i+1);
        QPoint delta0 = p1 - p0;
        QPoint delta1 = p2 - p1;

        QPoint pStart, pEnd;
        QPoint pCtrl = p1;
        int curveX = 10;
        int curveY = 10;

        int dRpX = 1;
        int dRpY = 1;
        if( delta0.y() == 0 )          // Horizontal line
        {
            if( delta0.x() > 0 ) dRpX = -1; // Right
            if( delta1.y() < 0 ) dRpY = -1; // Next line up

            int absY = abs( delta1.y() );
            if( absY < 2*curveY ) curveY = absY/2;

            int absX = abs( delta0.x() );
            if( absX < 2*curveX ) curveX = absX/2;

            pStart = p1 + QPoint( curveX*dRpX, 0    );
            pEnd   = p1 + QPoint( 0   , curveY*dRpY );
        }
        else if( delta0.x() == 0 )  // Vertical Line
        {
            if( delta0.y() > 0 ) dRpY = -1; // Down
            if( delta1.x() < 0 ) dRpX = -1; // Next line Left

            int absY = abs( delta0.y() );
            if( absY < 2*curveY ) curveY = absY/2;

            int absX = abs( delta1.x() );
            if( absX < 2*curveX ) curveX = absX/2;

            pStart = p1 + QPoint( 0   , curveY*dRpY );
            pEnd   = p1 + QPoint( curveX*dRpX, 0    );
        }
        m_path.lineTo( pStart );
        m_path.quadTo( pCtrl, pEnd );
        //p->drawPoint( pCtrl );

        p0 = p1;
    }
    m_path.lineTo( m_lastPoint );

    //p->drawRect( boundingRect() );
    //p->drawPath( shape() );

    QColor color;
    if( isSelected() ) color = QColor( Qt::darkGray );
    color = m_startPin->getColor();

    QPen pen( color, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen );
    p->drawPath( m_path );
    p->drawPath( m_path );
}
