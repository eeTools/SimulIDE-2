/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "rail.h"
#include "simulator.h"
#include "pin.h"

#include "doubleprop.h"

#define tr(str) simulideTr("Rail",str)

listItem_t Rail::libraryItem(){
    return {
        tr("Rail"),
        "Sources",
        "rail.png",
        "Rail",
        [](QString id){ return (Component*)new Rail( id ); } };
};

Rail::Rail( QString id )
    : Component( id )
    , m_out("outSour@"+id )
{
    m_area = QRect(-6,-9, 12, 9 );

    m_pin.emplace_back( new Pin( 270, QPoint(0, 8), "Pin@"+id, this ) );
    m_out.setAdmitance( 1e9 );

    setValLabelPos(-6,-20, 0 ); // x, y, rot
    setLabelPos(8,-10, 0);

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"), {
        new DoubProp<Rail>("Volt", tr("Voltage"),"V"
                        , this, &Rail::voltage, &Rail::setVoltage )
    }, 0} );

    setShowProp("Volt");
    setPropStr( "Volt", "5" );
}
Rail::~Rail() { }

void Rail::initialize()
{
    m_out.setNode( m_pin[0]->getNode() );
    m_out.setVoltage( m_voltage );
    m_changed = true;
}

void Rail::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;
    m_out.updtVoltage( m_voltage );
}

void Rail::setVoltage( double v )
{
    m_voltage = v;
    m_changed = true;
}

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
