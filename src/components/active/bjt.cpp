/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>
#include <QPainter>

#include "bjt.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"

#define tr(str) simulideTr("BJT",str)

listItem_t BJT::registerItem(){
    return {
        tr("BJT"),
        "Transistors",
        "bjt.png",
        "BJT",
        [](int id){ return (CompBase*)new BJT( id ); } };
}

BJT::BJT( int id )
   : Component( id )
   , Bjt()
{
    m_area =  QRectF(-12,-14, 28, 28 );
    setLabelPos( 18, 0, 0 );

    m_pin << new Pin(  90, QPoint( 8,-16), "PinC@"+id, this );
    m_pin << new Pin( 270, QPoint( 8, 16), "PinE@"+id, this );
    m_pin << new Pin( 180, QPoint(-16, 0), "PinB@"+id, this );

    Simulator::self()->addToUpdateList( this );

    /*addPropGroup( { tr("Main"), {
        new BoolProp<BJT>( "PNP"  , tr("PNP")      ,"" , this, &BJT::pnp      , &BJT::setPnp ),
        new DoubProp<BJT>( "Gain" , tr("Gain")     ,"" , this, &BJT::gain     , &BJT::setGain ),
        new DoubProp<BJT>( "Vcrit", tr("Threshold"),"V", this, &BJT::threshold, &BJT::setThreshold ),
    },0} );*/
}
BJT::~BJT(){}

void BJT::updateStep()
{
    if( Circuit::self()->animate() ) update();
    if( m_changed ) voltChanged(); // m_changed cleared at eDiode::voltChanged
}

void BJT::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w)
{
    Component::paint( p, o, w );
    
    //if( Circuit::self()->animate() && fabs(m_baseCurr) > 1e-4 ) p->setBrush( Qt::yellow );
    //else
    p->setBrush( Qt::white );

    p->drawEllipse( m_area );
    p->drawLine(-12, 0,-4, 0 );
    p->drawLine( -4,-8,-4, 8 );
    p->drawLine( -4,-4, 8,-12 );
    p->drawLine( -4, 4, 8, 12 );
    
    p->setBrush( Qt::black );
    if( m_PNP ){
        QPointF points[3] = {
        QPointF( 0.1, 6.8 ),
        QPointF( 2.4, 10 ),
        QPointF( 4, 7.5 ) };
        p->drawPolygon(points, 3);
    }else{
        QPointF points[3] = {
        QPointF( 6  , 10.7 ),
        QPointF( 2.4, 10 ),
        QPointF( 4  , 7.5 ) };
        p->drawPolygon(points, 3);
    }
    Component::paintSelected( p );
}
