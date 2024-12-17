/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <QDebug>

#include "node.h"
#include "pin.h"
#include "wire.h"
#include "circuit.h"

#include "doubleprop.h"
#include "pointprop.h"

Node::Node( QString id )
    : Component( id )
{
    setZValue( 101 );

    m_color = QColor( Qt::black );
    m_isBus = false;
    m_blocked = false;

    m_pin.resize( 3 );
    for( int i=0; i<3; i++ )
    {
        m_pin[i] = new Pin( 90*i, QPoint(0,0), "pin"+QString::number(i)+"@"+id, this );
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
        for( int i=0; i<3; i++) m_pin[i]->writeWireFlag( wireBus, true );
        m_isBus = true;
}   }

void Node::registerEnode( int enode, int n )
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
        Wire* wire = static_cast<Wire*>(m_pin[i]->wire());
        if( wire )
        {
            /// FIXME
            /*PinBase* coPin = m_pin[i]->conPin();
            if( coPin->component() == this ) // Wire betwen 2 Pins of this node
            {
                wire->setStartPin( nullptr );
                wire->setEndPin( nullptr );
                Circuit::self()->removeWire( wire );
                continue;
            }*/
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

    PinBase* pin0 = m_pin[c0];
    PinBase* pin1 = m_pin[c1];

    Wire* wire0 = static_cast<Wire*>(pin0->wire());
    Wire* wire1 = static_cast<Wire*>(pin1->wire());
    if( !wire0 || !wire1 ) return;

    if( pin1->conPin() != pin0 )
    {
        Wire* wire = new Wire( "Wire-"+Circuit::self()->newWireId() , pin0->conPin() );
        Circuit::self()->wireList()->append( wire );

        QStringList list0 = wire0->pointList();
        QStringList list1 = wire1->pointList();
        QStringList plist;

        if( pin0 == wire0->startPin() ){
            while( !list0.isEmpty() )
            {
                QString p2 = list0.takeLast();
                plist.append(list0.takeLast());
                plist.append(p2);
        }   }
        else while( !list0.isEmpty() ) plist.append( list0.takeFirst() );

        if( pin1 == wire1->endPin() ){
            while( !list1.isEmpty() )
            {
                QString p2 = list1.takeLast();
                plist.append(list1.takeLast());
                plist.append(p2);
        }   }
        else while( !list1.isEmpty() ) plist.append( list1.takeFirst() );

        wire->setPointList( plist );
        wire->closeCon( pin1->conPin() );
        if( this->isSelected() ) wire->select( true );
    }
    wire0->setStartPin( nullptr );
    wire0->setEndPin( nullptr );
    Circuit::self()->removeWire( wire0 );

    wire1->setStartPin( nullptr );
    wire1->setEndPin( nullptr );
    Circuit::self()->removeWire( wire1 );

    m_blocked = false;
}

void Node::setHidden( bool hid, bool , bool )
{
    m_hidden = hid;

    for( PinBase* pin : m_pin ) pin->setVisible( !hid );
    this->setVisible( !hid );
}

void Node::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    if( m_hidden ) return;

    //p->setBrush( Qt::blue );
    //p->drawRect( boundingRect() );

    Component::paint( p, o, w);

    if( m_isBus ) p->drawEllipse( QPointF(0,0), 1.8, 1.8  );
    else          p->drawEllipse( QPointF(0,0), 1.4, 1.4 );

    Component::paintSelected( p );
}
