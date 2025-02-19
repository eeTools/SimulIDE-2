/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "capacitor.h"
#include "pin.h"

#define tr(str) simulideTr("Capacitor",str)

listItem_t Capacitor::registerItem(){
    return {
        tr("Capacitor"),
        "Reactive",
        "capacitor.png",
        "Capacitor",
        [](int id){ return (CompBase*)new Capacitor( id ); } };

};

Capacitor::Capacitor( int id )
         : Component( id )
         , Capacitance()
{
    m_area = QRectF(-10,-8, 20, 16 );

    m_pin << new Pin( 180, QPoint(-16, 0 ), "lPin@"+id, this, 12 );
    m_pin << new Pin( 0,   QPoint( 16, 0 ), "rPin@"+id, this, 12 );

    setValLabelPos(-16, 6, 0 );
    setLabelPos(-16,-24, 0 );

    /*addPropGroup( { tr("Main"), {
        new DoubProp( "Capac", tr("Capacitance"), this, { &m_capacitance, "F", P_DOUBLE, 0, 0 } ),
//new DoubProp<CapacitorBase>( "Resist" , tr("Resistance")      ,"Ω"    , this, &CapacitorBase::resist  , &CapacitorBase::setResist ),
//new DoubProp<CapacitorBase>( "InitVolt", tr("Initial Voltage"), "V"    , this, &CapacitorBase::initVolt, &CapacitorBase::setInitVolt ),
//new IntProp <CapacitorBase>( "AutoStep", tr("Auto Step")      ,"_Steps", this, &CapacitorBase::autoStep, &CapacitorBase::setAutoStep,0,"uint" )
    },0 } );
    setShowProp("Capac");
    //setPropStr( "Capac", "10 µF" );*/
}
Capacitor::~Capacitor(){}

void Capacitor::initialize()
{
    m_node0 = m_pin[0]->getNode();
    m_node1 = m_pin[1]->getNode();
}

void Capacitor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    QPen pen = p->pen();
    pen.setWidth(3);
    p->setPen(pen);

    p->drawLine(-3,-6,-3, 6 );
    p->drawLine( 3,-6, 3, 6 );

    Component::paintSelected( p );
}
