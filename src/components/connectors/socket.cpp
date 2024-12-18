/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "socket.h"
#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"

#define tr(str) simulideTr("Socket",str)

listItem_t Socket::registerItem(){
    return {
        tr("Socket"),
        "Connectors",
        "socket.png",
        "Socket",
        [](QString id){ return (CompBase*)new Socket( id ); } };
}

Socket::Socket( QString id )
      : ConnBase( id )
{
    setZValue(-2 );

    m_pinType = Pin::pinSocket;

    m_color = QColor( 50, 50, 70 );

    /// Simulator::self()->addToSocketList( this );
    Simulator::self()->addToUpdateList( this );
}
Socket::~Socket()
{
    /// Simulator::self()->remFromSocketList( this );
}

void Socket::updateStep()
{
    updateConnections( false );
}

void Socket::updateConnections( bool connect )
{
    for( int i=0; i<m_size; i++ )
    {
        if( !m_sockPins[i]->wire() )
        {
            Pin* pin = m_sockPins[i]->connectPin( false );
            if( pin ){
                if( connect )
                {
                    qDebug()<<"Connecting"<< m_sockPins[i]->pinId()<<"To"<<pin->pinId();
                    m_connPins[i] = pin;
                    m_sockPins[i]->setConPin( pin );
                    pin->setConPin( m_sockPins[i] );
                }
                else if( !m_connPins[i] ) CircuitWidget::self()->powerCircOff();
            }
            else{
                if( m_connPins[i] )
                {
                    m_sockPins[i]->setConPin( nullptr );
                    m_connPins[i]->setConPin( nullptr );
                    m_connPins[i] = nullptr;
                    CircuitWidget::self()->powerCircOff();
                }
            }
        }
    }
}

void Socket::updatePixmap()
{
    int angle = this->rotation();
    switch( angle ) {
        case 0:
        case 180:
        case -180:
            m_pinPixmap.load( ":/socket_V.png" );
            m_pinPixmap = m_pinPixmap.transformed( QTransform().rotate(angle) );
            break;
        case 90:
        case -90:
            m_pinPixmap.load( ":/socket_H.png" );
            m_pinPixmap = m_pinPixmap.transformed( QTransform().rotate(angle-180) );
            break;
        default: break;
    }
}
