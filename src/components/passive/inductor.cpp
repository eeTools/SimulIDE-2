/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <math.h>

#include "inductor.h"
#include "pin.h"

#include "doubleprop.h"
#include "intprop.h"

#define tr(str) simulideTr("Inductor",str)

listItem_t Inductor::libraryItem(){
    return {
        tr("Inductor"),
        "Reactive",
        "inductor.png",
        "Inductor",
        [](QString id){ return (Component*)new Inductor( id ); } };
}

Inductor::Inductor( QString id )
        : Component( id )
        , Inductance( id )
{
    m_area = QRectF(-10,-6, 20, 12 );

    m_pin.resize(2);
    m_pin[0] = new Pin( 180, QPoint(-16, 0 ), "lPin@"+id, this);
    m_pin[1] = new Pin( 0,   QPoint( 16, 0 ), "rPin@"+id, this);

    m_pin[0]->setLength( 4 );
    m_pin[1]->setLength( 4 );

    //m_inductance = 1; // H

    addPropGroup( { tr("Main"), {
//new DoubProp<Inductor>( "Inductance", tr("Inductance")     , "H"     , this, &Inductor::value,    &Inductor::setValue ),
//new DoubProp<Inductor>( "Resistance", tr("Resistance")     , "Ω"     , this, &Inductor::resist  , &Inductor::setResist ),
//new DoubProp<Inductor>( "InitVolt"  , tr("Initial Current"), "A"     , this, &Inductor::initCurr, &Inductor::setInitCurr ),
//new IntProp <Inductor>( "AutoStep"  , tr("Auto Step")      , "_Steps", this, &Inductor::autoStep, &Inductor::setAutoStep,0,"uint" )
    },0 } );

    //setShowProp("Inductance");
    //setPropStr( "Inductance", "1" );
}
Inductor::~Inductor(){}

void Inductor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    QPen pen = p->pen();
    pen.setWidth( 2 );
    p->setPen( pen );
    
    p->drawArc( QRectF(-12,-4.5, 10, 10 ),-45*16 , 220*16 );
    p->drawArc( QRectF( -5,-4.5, 10, 10 ), 225*16,-270*16 );
    p->drawArc( QRectF(  2,-4.5, 10, 10 ), 225*16,-220*16 );

    Component::paintSelected( p );
}