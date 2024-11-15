/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "gate_or.h"

#include "intprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("OrGate",str)

listItem_t OrGate::libraryItem(){
    return {
        tr("Or Gate"),
        "Gates",
        "orgate.png",
        "OrGate",
        [](QString type, QString id){ return (Component*)new OrGate( type, id ); } };
}

OrGate::OrGate( QString type, QString id )
      : Gate( type, id, 2 )
{
    addPropGroup( { tr("Electric"),
        IoComponent::inputProps()
        +QList<ComProperty*>({
        new BoolProp<OrGate>("Invert_Inputs", tr("Invert Inputs"),""
                            , this, &OrGate::invertInps, &OrGate::setInvertInps, propNoCopy ),

        new IntProp <OrGate>("Num_Inputs", tr("Input Size"),"_Inputs"
                            , this, &OrGate::numInps, &OrGate::setNumInputs, propNoCopy,"uint" )
                    })
        + Gate::outputProps()
      //+ IoComponent::outputType()
        +QList<ComProperty*>({
        new BoolProp<OrGate>("Open_Collector", tr("Open Drain"), ""
                                 , this, &IoComponent::openCol, &IoComponent::setOpenCol, propNoCopy )
                    })
                    ,0 } );

    addPropGroup( { tr("Timing"), IoComponent::edgeProps(),0 } );
}
OrGate::~OrGate(){}

bool OrGate::calcOutput( int inputs ) { return (inputs > 0); }

QPainterPath OrGate::shape() const
{
    QPainterPath path;
    QVector<QPointF> points;
    
    int size = (int)m_inPin.size()*4;
    
    points << QPointF(-14,-size-1 )
           << QPointF(-10,-size+4 )
           << QPointF( -9, 0 )
           << QPointF(-10, size-4  )
           << QPointF(-14, size+1 )
           << QPointF(  0, size )
           << QPointF( 10, 8  )
           << QPointF( 10,-8  )
           << QPointF(  0,-size );
        
    path.addPolygon( QPolygonF(points) );
    path.closeSubpath();
    return path;
}

void OrGate::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    int endY = m_area.height()/2;
    int endX = m_area.width()/2;

    QPainterPath path;
    path.moveTo(-10,-endY );
    path.quadTo( QPoint( endX,-endY ), QPoint( endX, 0    ) );
    path.quadTo( QPoint( endX, endY ), QPoint( -10 , endY ) );
    path.quadTo( QPoint( -6  , 0    ), QPoint( -10 ,-endY ) );
    p->drawPath( path );

    /*QPen pen2( Qt::red, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen2 );
    p->drawPath( shape() );

    QPen pen3( Qt::blue, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen3 );
    p->drawRect( m_area );*/

    Component::paintSelected( p );
}
