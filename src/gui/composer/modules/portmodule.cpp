/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "portmodule.h"
#include "pinbase.h"
#include "fcomponent.h"

PortModule::PortModule( int id )
          : Module( id )
          , m_inputSlot("input" , hookInputInt )
          , m_outSignal("output", hookOutputInt )
{
    m_outSignal.setIntData( &m_state );
    m_slots.emplace_back( &m_inputSlot );
    m_signals.emplace_back( &m_outSignal );

    m_size = 0;
    m_position = 0;
    m_direction = 0;
    m_side = Right;
    m_sideStr = "Right";

    m_sideList = QStringList()<<"Right"<<"Left"<<"Top"<<"Bottom";

    /*addPropGroup( { "Main",
    {
        new StrProp("side", "Side", m_sideList.join(","), this,0,"enum" ),
        new IntProp("pos", "Offset", "", this, 0 ),
        new IntProp("dir", "Direction", "", this, 0 ),
        new IntProp("size", "Pins", "", this, propSlot ),
        new StrProp("pinnames", "Pin Names", "", this,0  ),
    },0} );*/
}
PortModule::~PortModule(){;}

void PortModule::setComponent( fComponent* c )
{
    Module::setComponent( c );
    m_component->addPort( this );
}

void PortModule::setSize( int size )
{
    if     ( size < 1  ) size = 1;
    else if( size > 32 ) size = 32;

    if( size == m_size ) return;

    if( size > m_size ) // Add pins
    {
        QString id = m_id+"@"+m_component->getUid();
        for( int i=m_size; i<size; ++i )
        {
            PinBase* pin = addPin( "Pin"+QString::number(i)+"@"+id );
            m_pins.append( pin );
        }
    }else               // Remove pins
    {
        for( int i=size; i<m_size; ++i )
        {
            PinBase* pin = m_pins.takeLast();
            m_component->deletePin( pin );
        }
    }
    m_size = size;

    setPinNames( m_pinNames );

    m_component->upDateShape();
}

void PortModule::setSide( QString side )
{
    if( !m_sideList.contains( side )) side = "Left";
    if( side == m_sideStr ) return;

    m_sideStr = side;
    m_side = (side_t)m_sideList.indexOf( side );

    m_component->upDateShape();
}

void PortModule::setPosition( int pos )
{
    if( pos < -1 ) pos = -1;
    //if( m_position == pos ) return;
    m_position = pos;
    m_component->upDateShape();
}

void PortModule::updatePosition( double pos )
{
    if( m_pins.isEmpty() ) return;

    int x=0, y=0, angle=0;
    int pinLenght = m_pins.at(0)->length();
    int width  = m_component->width()*8/2;
    int height = m_component->height()*8/2;
    bool vertical = true;

    switch( m_side )
    {
        case Right:{
            x = width+pinLenght;
            y = -height+pos*8;
        }break;
        case Left:{
            x = -width-pinLenght;
            y = -height+pos*8;
            angle = 180;
        }break;
        case Top:{
            x = -width+pos*8;
            y = -height-pinLenght;
            angle = 90;
            vertical = false;
        }break;
        case Bottom:{
            x = -width+pos*8;
            y = height+pinLenght;
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

void PortModule::setPinNames( QString n )
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

void PortModule::remove()
{
    for( PinBase* pin : m_pins ) m_component->deletePin( pin );
}
