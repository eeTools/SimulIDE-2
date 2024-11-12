/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <QDebug>

#include "node.h"
#include "wire.h"
#include "circuit.h"

#include "doubleprop.h"
#include "pointprop.h"
#include "pin.h"

Node::Node( QString type, QString id )
    : Component( type, id )
{
    setZValue( 101 );

    m_color = QColor( Qt::black );
    m_isBus = false;
    m_blocked = false;

    m_pin.resize( 3 );
    for( int i=0; i<3; i++ )
    {
        m_pin[i] = new Pin( 90*i, QPoint(0,0), id+"-"+uchar(48+i), i, this );
        m_pin[i]->setLength( 0 );
    }
    remPropGroup( "CompGraphic" );
    addPropGroup( { "CompGraphic", {
new PointProp <Component>( "pos","","",this, &Component::position, &Component::setPosition )
    },0} );
}
Node::~Node(){}

void Node::pinMessage( int rem ) // Called by pin
{
    if     ( rem == 1 ) checkRemove();
    else if( rem == 2 ) // Propagate Is Bus
    {
        for( int i=0; i<3; i++) m_pin[i]->setIsBus( true );
        m_isBus = true;
}   }

void Node::registerEnode( eNode* enode, int n )
{
    for( int i=0; i<3; i++ )
        if( m_pin[i]->conPin() ) m_pin[i]->registerPinsW( enode, n );
}

bool Node::checkRemove() // Only remove if there are less than 3 connectors
{
    if( m_blocked ) return false;

    int con[2] = { 0, 0 };
    int conectors = 0;
    int conecteds = 0;

    for( int i=0; i<3; i++ )
    {
        Wire* wire = m_pin[i]->wire();
        if( wire )
        {
            Pin* coPin = m_pin[i]->conPin();
            if( coPin->component() == this ) // Wire betwen 2 Pins of this node
            {
                wire->setStartPin( nullptr );
                wire->setEndPin( nullptr );
                Circuit::self()->removeWire( wire );
                continue;
            }
            if( conecteds == 0 ) { conecteds++; con[0] = i; }
            else con[1] = i;
            conectors++;
    }   }
    if( conectors < 3 )
    {
        if( conectors == 2) joinConns( con[0], con[1] );  // 2 Conn
        else                m_pin[con[0]]->removeWire();

        Circuit::self()->removeNode( this );
        return true;
    }
    else return false;
}

void Node::joinConns( int c0, int c1 )
{
    m_blocked = true;
    Pin* pin0 = m_pin[c0];
    Pin* pin1 = m_pin[c1];

    Wire* wire0 = pin0->wire();
    Wire* wire1 = pin1->wire();
    if( !wire0 || !wire1 ) return;

    Pin* endPin = wire1->endPin();

    if( pin0 == wire0->startPin() ) wire0->updateConRoute( pin0 );             // Forze Node pin to be endPin
    if( pin1 == endPin            ) wire1->updateConRoute( wire1->startPin() ); // Forze Node pin to be startPin

    QList<QPoint> pv1 = wire1->pointVector();
    pv1.takeFirst();
    QList<QPoint> pointVector = wire0->pointVector() + pv1;
    pointVector.append( endPin->scenePos().toPoint() );

    Circuit::self()->removeWire( wire1 );

    wire0->setPointVector( pointVector );
    wire0->setEndPin( endPin );
    wire0->closeCon( endPin );

    m_blocked = false;
}

void Node::setHidden( bool hid, bool , bool )
{
    m_hidden = hid;

    for( Pin* pin : m_pin ) pin->setVisible( !hid );
    this->setVisible( !hid );
}

void Node::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    if( m_hidden ) return;

    //p->setBrush( Qt::blue );
    //p->drawRect( boundingRect() );

    Component::paint( p, option, widget );
    
    if( m_isBus ) p->drawEllipse( QPointF(0,0), 1.8, 1.8  );
    else          p->drawEllipse( QPointF(0,0), 1.4, 1.4 );

    Component::paintSelected( p );
}
