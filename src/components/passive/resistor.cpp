/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <QDebug>

#include "resistor.h"
#include "pin.h"

#define tr(str) simulideTr("Resistor",str)

listItem_t Resistor::registerItem(){
    return {
        tr("Resistor"),
        "Resistors",
        "resistor.png",
        "Resistor",
        [](int id){ return (CompBase*)new Resistor( id ); } };
}

Resistor::Resistor( int id )
        : Component( id )
{
    m_area = QRectF(-11,-4.5, 22, 9 );

    m_pin << new Pin( 180, QPoint(-16, 0 ), "lPin@"+id, this, 5 );
    m_pin << new Pin( 0,   QPoint( 16, 0 ), "rPin@"+id, this, 5 );

    setValLabelPos(-16, 6, 0 );
    setLabelPos(-16,-24, 0 );

    /*m_res = 50;
    qDebug() << "sizeof  groupFlags" << sizeof( groupFlags );
    qDebug() << "sizeof  property_t" << sizeof( property_t );
    qDebug() << "sizeof  void*" << sizeof( void* );*/

    setResistance( 100 );

    addPropGroup( { tr("Main"), {}, 0 },
    {
        {"res", tr("Resistance"), "Ω", &m_resistVal, P_Double, P_NoSet }
    });

    m_resistProp = getPropertyId("res");
}
Resistor::~Resistor(){}

void Resistor::initialize()
{
    m_resistance.m_node0 = m_pin[0]->getNode();
    m_resistance.m_node1 = m_pin[1]->getNode();
}

void Resistor::setResistance( double r )
{
    m_resistVal = r;
    m_resistance.setResistance( r );
}

void Resistor::setValue( const uint8_t idInt, const value_t &val )
{
    if( idInt == m_resistProp ) setResistance( *val.dblVal );
    else                        Component::setValue( idInt, val );
}

void Resistor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );
    p->drawRect( m_area );

    Component::paintSelected( p );
}
