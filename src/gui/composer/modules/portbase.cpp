/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "portbase.h"
#include "pinbase.h"
#include "fcomponent.h"

PortBase::PortBase( QString name )
        : Module( name )
{
    m_size = 0;
    m_position = 0;
    m_direction = 0;
    m_side = Right;
}
PortBase::~PortBase(){;}

void PortBase::setComponent( fComponent* c )
{
    Module::setComponent( c );
    m_component->addPort( this );
}

void PortBase::setSize( int size )
{
    if     ( size < 1  ) size = 1;
    else if( size > 32 ) size = 32;

    if( size == m_size ) return;

    if( size > m_size ) // Add pins
    {
        QString id = m_id+"@"+m_component->getUid();
        for( int i=m_size; i<size; ++i )
        {
            //IoPin* pin = new IoPin( 0, QPoint(0, 0), "Pin"+QString::number(i)+"@"+id, m_component );
            PinBase* pin = addPin( "Pin"+QString::number(i)+"@"+id );
            m_pins.append( pin );
        }
    }else               // Remove pins
    {
        for( int i=size; i<m_size; ++i )
        {
            m_component->deletePin( m_pins.at(i) );
            m_pins.takeLast();
        }
    }
    m_size = size;

    setPinNames( m_pinNames );

    m_component->upDateShape();
}

void PortBase::setSide( QString side )
{
    if( !m_sideList.contains( side )) side = "Left";
    if( side == m_sideStr ) return;

    m_sideStr = side;
    m_side = (side_t)m_sideList.indexOf( side );

    m_component->upDateShape();
}

void PortBase::setPosition( int pos )
{
    if( pos < -1 ) pos = -1;
    //if( m_position == pos ) return;
    m_position = pos;
    m_component->upDateShape();
}

void PortBase::updatePosition( double pos )
{
    int x=0, y=0, angle=0;
    int width  = m_component->width()*8/2;
    int height = m_component->height()*8/2;
    bool vertical = true;

    switch( m_side )
    {
        case Right:{
            x = width+8;
            y = -height+pos*8;
        }break;
        case Left:{
            x = -width-8;
            y = -height+pos*8;
            angle = 180;
        }break;
        case Top:{
            x = -width+pos*8;
            y = -height-8;
            angle = 90;
            vertical = false;
        }break;
        case Bottom:{
            x = -width+pos*8;
            y = height+8;
            angle = 270;
            vertical = false;
        }break;
    }
    for( PinBase* pin : m_pins )
    {
        pin->setX( x );
        pin->setY( y );
        pin->setPinAngle( angle );

        if( vertical ) y += 8;
        else           x += 8;
    }
}

void PortBase::setPinNames( QString n )
{
    //if( m_pinNames == n ) return;
    m_pinNames = n;

    QStringList pNames = n.split(",");
    for( PinBase* pin : m_pins)
    {
        QString pinName = pNames.isEmpty() ? "" : pNames.takeFirst();
        pin->setLabelText( pinName );
        pin->setFlag( QGraphicsItem::ItemStacksBehindParent, pinName.isEmpty() );
    }
}
