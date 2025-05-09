/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "gate_and.h"

#include "intprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("AndGate",str)

listItem_t AndGate::libraryItem(){
    return {
        tr("And Gate" ),
        "Gates",
        "andgate.png",
        "AndGate",
        [](QString id){ return (Component*)new AndGate("AndGate", id ); } };
}

AndGate::AndGate( QString type, QString id )
       : Gate( type, id, 2 )
{
    addPropGroup( { tr("Electric"),
        IoComponent::inputProps()
        +QList<ComProperty*>({
        new IntProp <AndGate>("Num_Inputs", tr("Input Size"),""
                             , this, &AndGate::numInps, &AndGate::setNumInputs, propNoCopy,"uint" ),

        new BoolProp<AndGate>("Invert_Inputs", tr("Invert Inputs"),""
                             , this, &AndGate::invertInps, &AndGate::setInvertInps, propNoCopy )
                        })
        +Gate::outputProps()
      //+ IoComponent::outputType()
        +QList<ComProperty*>({
        new BoolProp<AndGate>("Open_Collector", tr("Open Drain"), ""
                                 , this, &IoComponent::openCol, &IoComponent::setOpenCol, propNoCopy )
                    })
                    ,0 } );

    addPropGroup( { tr("Timing"), IoComponent::edgeProps(),0 } );
}
AndGate::~AndGate(){}

QPainterPath AndGate::shape() const
{
    int size = (int)m_inPin.size()*4;
    QPainterPath path;
    QVector<QPointF> points;
    points << QPointF( -9,-size )
           << QPointF( -9, size )
           << QPointF(  0, size-2 )
           << QPointF( 10, 8 )
           << QPointF( 10,-8 )
           << QPointF(  0,-size+2 );
        
    path.addPolygon( QPolygonF(points) );
    path.closeSubpath();
    return path;
}

void AndGate::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );
    p->drawChord( -27, m_area.y(), 36, m_area.height(), -1440/*-16*90*/, 2880/*16*180*/ );

    Component::paintSelected( p );
}
