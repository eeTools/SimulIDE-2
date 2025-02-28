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
    , m_voltage( this, "volt", tr("Voltage"),"V", 5, P_NoSet )
{
    m_area = QRect(-6,-9, 12, 9 );

    m_out = new OutputPin( 270, QPoint(0, 8), "Pin@"+QString::number(id), this );
    m_pin << m_out;
    m_out->setOutputImp( 1e-9 );

    setValLabelPos(-6,-20, 0 ); // x, y, rot
    setLabelPos(8,-10, 0);

    Simulator::self()->addToUpdateList( this );

    //m_voltage = 5;

    addPropGroup( { tr("Main"), {&m_voltage}, 0} );

    ///setShowProp("volt");
    //setPropStr( "Volt", "5" );
}
Rail::~Rail() { }

void Rail::initialize()
{
    /// m_out->setVoltage( m_voltage );
    m_changed = true;
}

void Rail::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;
    m_out->setVoltage( m_voltage.get() );
}

void Rail::propertyChanged( const ComProperty* prop )
{
    if( prop == &m_voltage )
    {
        m_changed = true;
    }
    else Component::propertyChanged( prop );
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
