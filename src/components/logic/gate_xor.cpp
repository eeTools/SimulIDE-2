/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "gate_xor.h"

#include "boolprop.h"

#define tr(str) simulideTr("XorGate",str)

listItem_t XorGate::libraryItem(){
    return {
        tr("Xor Gate" ),
        "Gates",
        "xorgate.png",
        "XorGate",
        [](QString type, QString id){ return (Component*)new XorGate( type, id ); } };
}

XorGate::XorGate( QString type, QString id )
       : Gate( type, id, 2 )
{
    addPropGroup( { tr("Electric"), IoComponent::inputProps()
        +QList<ComProperty*>({
        new BoolProp<XorGate>("Invert_Inputs", tr("Invert Inputs"),""
                             , this, &XorGate::invertInps, &XorGate::setInvertInps,propNoCopy )
                            })
        + Gate::outputProps()
      //+ IoComponent::outputType()
        +QList<ComProperty*>({
        new BoolProp<XorGate>("Open_Collector", tr("Open Drain"), ""
                               , this, &IoComponent::openCol, &IoComponent::setOpenCol, propNoCopy )
                    })
                    ,0 } );

    addPropGroup( { tr("Timing"), IoComponent::edgeProps(),0 } );
}
XorGate::~XorGate(){}

bool XorGate::calcOutput( int inputs ) { return (inputs == 1); }

QPainterPath XorGate::shape() const
{
    QPainterPath path;
    
    QVector<QPointF> points;
    
    int size = (int)m_inPin.size()*4;
    
    points << QPointF(-14,-size-1 )
           << QPointF(-10,-size+4  )
           << QPointF(-10, size-4  )
           << QPointF(-14, size+2 )
           << QPointF(  0, size )
           << QPointF( 10, 8  )
           << QPointF( 10,-8  )
           << QPointF(  0,-size );
        
    path.addPolygon( QPolygonF(points) );
    path.closeSubpath();
    return path;
}

void XorGate::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    int endY = m_area.height()/2;
    int endX = m_area.width()/2;

    QPainterPath path;
    path.moveTo(-7,-endY );
    path.quadTo( QPoint( endX,-endY ), QPoint( endX, 0   ) );
    path.quadTo( QPoint( endX, endY ), QPoint( -7 , endY ) );
    path.quadTo( QPoint( -3  , 0    ), QPoint( -7 ,-endY ) );
    p->drawPath( path );

    p->setBrush( Qt::NoBrush );

    QPainterPath curve;
    curve.moveTo(-10, endY );
    curve.quadTo( QPointF(-5.5, 0 ), QPoint(-10,-endY ) );
    p->drawPath( curve );

    /*QPen pen2( Qt::red, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen2 );
    p->drawPath( shape() );

    QPen pen3( Qt::blue, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen3 );
    p->drawRect( m_area );*/

    Component::paintSelected( p );
}
