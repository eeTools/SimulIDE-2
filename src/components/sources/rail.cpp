/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "rail.h"
#include "iopin.h"
#include "simulator.h"

#define tr(str) simulideTr("Rail",str)

listItem_t Rail::registerItem(){
    return {
        tr("Rail"),
        "Sources",
        "rail.png",
        "Rail",
        [](int id){ return (CompBase*)new Rail( id ); } };
}

Rail::Rail( int id )
    : Component( id )
{
    m_area = QRect(-6,-9, 12, 9 );

    m_out = new OutputPin( 270, QPoint(0, 8), "Pin@"+id, this );
    m_pin << m_out;
    m_out->setOutputImp( 1e-9 );

    setValLabelPos(-6,-20, 0 ); // x, y, rot
    setLabelPos(8,-10, 0);

    Simulator::self()->addToUpdateList( this );

    m_voltage = 5;

    addPropGroup( { tr("Main"), {}, 0},
    {
        {"volt", tr("Volt"),"V", &m_voltage, P_Double, P_NoSet }
    });

    m_voltProp = getPropertyId("volt");

    ///setShowProp("volt");
    //setPropStr( "Volt", "5" );
}
Rail::~Rail() { }

void Rail::initialize()
{
    m_out->setVoltage( m_voltage );
    m_changed = true;
}

void Rail::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;
    m_out->setVoltage( m_voltage );
}

void Rail::setValue( const uint8_t idInt, const value_t &val )
{
    if( idInt == m_voltProp )
    {
        m_voltage = *val.dblVal;
        m_changed = true;
    }
    else Component::setValue( idInt, val );
}

/*void Rail::setVoltage( double v )
{
    m_voltage = v;
    m_changed = true;
}*/

void Rail::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );
    p->setBrush( QColor( 255, 166, 0 ) );

    static const QPointF points[4] = {
        QPointF( 1, 1 ),
        QPointF(-1, 1 ),
        QPointF(-6.5,-9 ),
        QPointF( 6.5,-9 )            };

    p->drawPolygon( points, 4 );

    Component::paintSelected( p );
}
