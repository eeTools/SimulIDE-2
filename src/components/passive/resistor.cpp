/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "resistor.h"
#include "pin.h"

#include "doubleprop.h"

#define tr(str) simulideTr("Resistor",str)

listItem_t Resistor::registerItem(){
    return {
        tr("Resistor"),
        "Resistors",
        "resistor.png",
        "Resistor",
        [](QString id){ return (CompBase*)new Resistor( id ); } };
}

Resistor::Resistor( QString id )
        : Component( id )
        , Resistance( id )
{
    m_area = QRectF(-11,-4.5, 22, 9 );

    m_pin << new Pin( 180, QPoint(-16, 0 ), "lPin@"+id, this, 5 );
    m_pin << new Pin( 0,   QPoint( 16, 0 ), "rPin@"+id, this, 5 );

    setValLabelPos(-16, 6, 0 );
    setLabelPos(-16,-24, 0 );

    addPropGroup( { tr("Main"), {
new DoubProp<Resistor>( "Resist", tr("Resistance"), "Ω", this, &Resistor::getResistance, &Resistor::setResistance )
    }, 0 } );
}
Resistor::~Resistor(){}

void Resistor::initialize()
{
    m_nodes[0] = m_pin[0]->getNode();
    m_nodes[1] = m_pin[1]->getNode();
}

void Resistor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );
    p->drawRect( m_area );

    Component::paintSelected( p );
}
