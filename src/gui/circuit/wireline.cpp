/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>

#include "wireline.h"
#include "wire.h"
#include "circuitview.h"
#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
#include "node.h"
#include "utils.h"

#define tr(str) simulideTr("WireLine",str)
 
WireLine::WireLine( int x1, int y1, int x2, int y2, Wire* wire )
        : QGraphicsItem()
{
    m_wire = wire;
    
    m_prevLine = nullptr;
    m_nextLine = nullptr;

    m_p1X = x1;
    m_p1Y = y1;
    m_p2X = x2;
    m_p2Y = y2;

    m_isBus  = false;
    m_moveP1 = false;
    m_moveP2 = false;
    m_moving = false;
    m_animateCurrent = false;

    this->setFlag( QGraphicsItem::ItemIsSelectable, true );

    setCursor( Qt::CrossCursor );
    setZValue( 100 );
    updatePos();
}
WireLine::~WireLine(){}

QRectF WireLine::boundingRect() const
{
    int dy = m_p2Y-m_p1Y;
    int dx = m_p2X-m_p1X;
    int p =-1;
    int d = 2;

    if    ( dx != 0
         && dy != 0 ) return QRect( 0   , 0   , dx  , dy );
    else if( dx > 0 ) return QRect(-1   ,-2   , dx+d, 4 );
    else if( dx < 0 ) return QRect( dx+p,-2   ,-dx+d, 4 );
    else if( dy > 0 ) return QRect(-2   ,-1   , 4   , dy+d );
    else if( dy < 0 ) return QRect(-2   , dy+p, 4   ,-dy+d );
    else              return QRect( 0   , 0   , 0   , 0 );
}

void WireLine::sSetP1( QPoint point )
{
    prepareGeometryChange();
    m_p1X = point.x();
    m_p1Y = point.y();
    updatePos();
}

void WireLine::sSetP2( QPoint point )
{
    prepareGeometryChange();
    m_p2X = point.x();
    m_p2Y = point.y();
    updatePos();
}

void WireLine::moveSimple( QPointF delta )
{
    bool deltaH  = fabs( delta.x() )> 0;
    bool deltaV  = fabs( delta.y() )> 0;
    
    prepareGeometryChange();

    m_p1X = m_p1X + delta.x();
    m_p1Y = m_p1Y + delta.y();
    m_p2Y = m_p2Y + delta.y();
    m_p2X = m_p2X + delta.x();

    bool isHoriz = ( dy() == 0 ) && ( dx() != 0 );
    
    if( m_prevLine && !(m_prevLine->isSelected()) )
    {
        m_prevLine->moveLine( delta.toPoint() );
        
        if( (  isHoriz && deltaV )
          ||( !isHoriz && deltaH ))
            m_prevLine->sSetP2( QPoint( m_p1X, m_p1Y) );
            
        m_prevLine->updatePos();
        m_prevLine->updatePrev();
    }
    if( m_nextLine && !(m_nextLine->isSelected()) )
    {
        m_nextLine->moveLine( delta.toPoint() ); 
        
        if( (  isHoriz && deltaV )
          ||( !isHoriz && deltaH ))
            m_nextLine->sSetP1( QPoint( m_p2X, m_p2Y) );
            
        m_nextLine->updatePos();
        m_nextLine->updateNext();
    }
    updatePos();
}

bool WireLine::isDiagonal() { return ( fabs(m_p2X - m_p1X)>0 && fabs(m_p2Y - m_p1Y)>0 ); }

void WireLine::move( QPointF delta )
{
    if( Circuit::self()->pasting() )
    {
        prepareGeometryChange();
        m_p1Y = m_p1Y + delta.y();
        m_p2Y = m_p2Y + delta.y();
        m_p1X = m_p1X + delta.x();
        m_p2X = m_p2X + delta.x();
        updatePos();

        return;
    }
    int myindex = m_wire->lineList()->indexOf( this );
    if( ( myindex == 0 ) || ( myindex == m_wire->lineList()->size()-1 ) )
        return;    //avoid moving first or last line

    moveLine( delta.toPoint() );
    updatePos();
    updateLines();
    m_wire->refreshPointList();
}

void WireLine::moveLine( QPoint delta )
{
    prepareGeometryChange();

    if( dx() != 0 ){
       m_p1Y = m_p1Y + delta.y();
       m_p2Y = m_p2Y + delta.y();
    }
    if( dy() != 0 ){
       m_p1X = m_p1X + delta.x();
       m_p2X = m_p2X + delta.x();
    }
}

void WireLine::updateNext()
{
    if( m_nextLine ){
        m_nextLine->sSetP1( QPoint( m_p2X, m_p2Y) );
        m_nextLine->updatePos();
    }
}

void WireLine::remove()
{ 
    if( !isSelected() ){
        Circuit::self()->clearSelection();
        setSelected( true );
    }
    Circuit::self()->removeItems();
}

void WireLine::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    if( event->button() == Qt::MidButton )                      // Move Line
    {
        event->accept();

        if     ( dy() == 0 ) CircuitView::self()->viewport()->setCursor( Qt::SplitVCursor );
        else if( dx() == 0 ) CircuitView::self()->viewport()->setCursor( Qt::SplitHCursor );
        else                 CircuitView::self()->viewport()->setCursor( Qt::SizeAllCursor );
    }
    else if( event->button() == Qt::LeftButton )
    {
        if( event->modifiers() == Qt::ControlModifier ) setSelected( !isSelected() ); // Select - Deselect
        
        else if( event->modifiers() & Qt::ShiftModifier ) // Move Corner
        {
            QPoint evPoint = toGrid( event->scenePos() ).toPoint();
            
            if     ( evPoint==p1() ) m_moveP1 = true;
            else if( evPoint==p2() ) m_moveP2 = true;
        }
        else{                                         // Connecting a wire here
            WireBase* wire = Circuit::self()->getNewWire();
            if( wire )     // Wire started at Pin is connecting here
            {

                if( wire == m_wire ) return;
                if( wire->wireFlags() != m_wire->wireFlags() ){ event->ignore(); return; } // Avoid connect Bus with no-Bus
            }
            QPoint point1 = toGrid( event->scenePos() ).toPoint();

            if( connectToWire( point1 ) ) event->accept();
            else                          event->ignore();
}   }   }

bool WireLine::connectToWire( QPoint point1 )
{
    int index;
    int myindex = m_wire->lineList()->indexOf( this );

    if((( dy() == 0 && fabs( point1.x()-m_p2X ) < 8 ) // point near the p2 corner
     || ( dx() == 0 && fabs( point1.y()-m_p2Y ) < 8 ) )
     && ( myindex != m_wire->lineList()->size()-1 ) )
    {
        if( myindex == m_wire->lineList()->size()-1 ) return false;
        point1 = p2();
        index = myindex+1;
    }
    else if((( dy() == 0 && fabs( point1.x()-m_p1X ) < 8 ) // point near the p1 corner
          || ( dx() == 0 && fabs( point1.y()-m_p1Y ) < 8 ) )
          && ( myindex != 0 ) )
    {
        if( myindex == 0 ) return false;
        point1 = p1();
        index = myindex;
    }
    else{                                           // split this line in two
        if( dy() == 0 ) point1.setY( m_p1Y );
        else            point1.setX( m_p1X );
        index = myindex+1;
        WireLine* line = new WireLine( point1.x(), point1.y(), m_p2X, p2().y(), m_wire );
        m_wire->addConLine( line, index );
    }
    if( Simulator::self()->isRunning() )  CircuitWidget::self()->powerCircOff();

    if( !Circuit::self()->getNewWire() ) Circuit::self()->beginUndoStep(); // A new Connector started here

    Node* node = new Node( Circuit::self()->newSceneId() );     // Now add the Node
    node->setPos( point1.x(), point1.y());
    Circuit::self()->addNode( node );

    if( Circuit::self()->getNewWire() )   // A Connector wants to connect here (ends in a node)
    {
        m_wire->splitCon( index, node->getPin(0), node->getPin(2) );
        Circuit::self()->closeWire( node->getPin(1), true );
    }
    else{                                    // A new Connector created here (starts in a node)
        Pin* pin = node->getPin(1);
        pin->setWireFlags( m_wire->m_wireFlags );
        Circuit::self()->startWire( pin, false );
        m_wire->splitCon( index, node->getPin(0), node->getPin(2) );
    }
    return true;
}

void WireLine::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
    event->accept();

    QPoint delta = toGrid( event->scenePos() ).toPoint() - toGrid(event->lastScenePos()).toPoint();

    if( !m_moving && !Circuit::self()->getNewWire() )
    {
        Circuit::self()->saveItemChange( m_wire->getUid(), "pointList", m_wire->pListStr() );
        m_moving = true;
    }

    if( event->modifiers() & Qt::ShiftModifier )          // Move Corner
    {
        if     ( m_moveP1 ) setP1( p1()+delta );
        else if( m_moveP2 ) setP2( p2()+delta );
    }else{
        int myindex = m_wire->lineList()->indexOf( this );

        if( myindex == 0 )
            m_wire->addConLine( p1().x(), p1().y(), p1().x(), p1().y(), myindex );

        else if( myindex == m_wire->lineList()->size()-1 )
            m_wire->addConLine( p2().x(), p2().y(), p2().x(), p2().y(), myindex + 1 );

        moveLine( delta );
    }
    updatePos();
    updateLines();
}

void WireLine::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
    event->accept();
    m_moveP1 = false;
    m_moveP2 = false;
    m_wire->remNullLines();
    m_moving = false;
}

void WireLine::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
    if( Circuit::self()->getNewWire() ) return;

    if( m_wire->endPin() )
    {
       event->accept();
       QMenu menu;

       QAction* removeAction = menu.addAction( tr("Remove") );
       QObject::connect( removeAction, &QAction::triggered, [=](){ remove(); } );

       menu.exec(event->screenPos());
}   }

void WireLine::animateLine()
{

    update();

        if( m_moving ) return;
    updtLength();
m_animateCurrent = true;
m_currentSpeed = 1;
    uint64_t time = Simulator::self()->circTime()/1e8;
m_step = time;
    time *= m_currentSpeed;
    time /= 80; // 50 ms FIXME: get FPS
    time = time%80;
    m_step = (double)time/10;
    //qDebug() <<"ConnectorLine::animate"<< m_lenght << time << m_step;
}

void WireLine::updtLength()
{
    int termX = m_p2X-m_p1X;
    int termY = m_p2Y-m_p1Y;
    m_lenght = std::fabs( std::sqrt( termX*termX + termY*termY) );
}

QPainterPath WireLine::shape() const
{
    int dy = m_p2Y-m_p1Y;
    int dx = m_p2X-m_p1X;
    int q = 0;
    int i = 0;

    if( this->cursor().shape() == Qt::ArrowCursor ) // New Connector
    {
        if     ( dx > 0 ) q = -3;
        else if( dx < 0 ) q =  3;
        if     ( dy > 0 ) i = -3;
        else if( dy < 0 ) i =  3;
    }

    QPainterPath path;
    QVector<QPointF> points;
    
    if( fabs(m_p2X - m_p1X) > fabs(m_p2Y - m_p1Y) )
    {
        points << mapFromScene( QPointF( m_p1X  , m_p1Y-2 ) )
               << mapFromScene( QPointF( m_p1X  , m_p1Y+2 ) )
               << mapFromScene( QPointF( m_p2X+q, m_p2Y+2 ) )
               << mapFromScene( QPointF( m_p2X+q, m_p2Y-2 ) );
    }else{
        points << mapFromScene( QPointF( m_p1X-2, m_p1Y   ) )
               << mapFromScene( QPointF( m_p1X+2, m_p1Y   ) )
               << mapFromScene( QPointF( m_p2X+2, m_p2Y+i ) )
               << mapFromScene( QPointF( m_p2X-2, m_p2Y+i ) );
    }
    path.addPolygon( QPolygonF(points) );
    path.closeSubpath();
    return path;
}

void WireLine::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    //pen.setColor( Qt::darkGray);
    //p->setPen( pen );

    QColor color;
    if( isSelected() ) color = QColor( Qt::darkGray );
    else if( m_isBus ) color =  Qt::darkGreen;
    else if( Circuit::self()->animate() )
    {
        if( m_wire->getVoltage() > 2.5 ) color = QColor( 200, 50, 50  );
        else                             color = QColor( 50,  50, 200 );
    }
    else color = QColor( 40, 40, 60 /*Qt::black*/ );

    QPen pen( color, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    //p->setBrush( Qt::green );
    //p->drawRect( boundingRect() );
    //p->setBrush( Qt::blue );
    //p->drawPath( shape() );

    if( m_isBus ) pen.setWidth( 3 );
    else if( m_animateCurrent ) pen.setWidthF( 2.5 );

    p->setPen( pen );
    p->drawLine( 0, 0, dx(), dy());

    if( m_isBus ) return;
    if( !m_animateCurrent ) return;
    if( !Simulator::self()->isRunning() ) return;
    p->setBrush( QColor( 200, 255, 50 ) );
    pen.setWidthF( 0.5 );
    p->setPen( pen );

    int dir = 1;
    if( dx() )
    {
        if( dx() < 0 ) dir = -1;

        for( double i=0; i+m_step<m_lenght; i+=8 )
            p->drawEllipse( QPointF( dir*(i+m_step), 0 ), 1.5, 1.5  );
    }
    if( dy() )
    {
        if( dy() < 0 ) dir = -1;

        for( double i=0; i+m_step<m_lenght; i+=8 )
            p->drawEllipse( QPointF( 0, dir*(i+m_step )), 1.5, 1.5  );
    }
}

