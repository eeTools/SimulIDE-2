/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>

#include "wire.h"
#include "wireline.h"
#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"
#include "utils.h"

Wire::Wire( int id, PinBase* startpin, PinBase* endpin )
    : WireBase( id, startpin )
{
    m_type = "Wire";
    m_freeLine = false;

    if( endpin ) closeCon( endpin );
    else m_endPin = nullptr;

    Circuit::self()->compMap()->insert( id, this );
}
Wire::~Wire()
{
    Circuit::self()->compMap()->remove( m_id );
}

void Wire::remNullLines()      // Remove lines with leght = 0 or aligned
{
    if( m_wireLineList.length() < 2 ) { refreshPointList(); return; }

    for( WireLine* line : m_wireLineList )
    {
        if( line->isDiagonal() ) continue;
        
        int index = m_wireLineList.indexOf( line );
        if( index < m_wireLineList.length()-1 )      //
        {
            WireLine* line2 = m_wireLineList.at( index+1 );
            
            if( line2->isDiagonal() ) continue;

            if( line->dx() == line2->dx() || line->dy() == line2->dy() ) // Lines aligned or null line
            {
                line2->sSetP1( line->p1() );
                remConLine( line  );
    }   }   }
    if( m_wireLineList.length() < 2 )
    {
        m_lastIndex = 0;
        m_actLine   = 0;
    }
    refreshPointList();
}

void Wire::remConLine( WireLine* line  )
{
    int index = m_wireLineList.indexOf( line );

    connectLines( index-1, index+1 );
    if( line->scene() ) Circuit::self()->removeItem( line );
    m_wireLineList.removeOne( line );
    if( m_actLine > 0 ) m_actLine -= 1;
}

void Wire::setPointListStr( QString pl )
{
    setPointList( pl.split(",") );
    //remNullLines();
}

void Wire::setPointList( QStringList plist )
{
    remLines();
    m_pointList = plist;

    int index = 0;
    int p1x = plist.at(0).toInt();
    int p1y = plist.at(1).toInt();
    int p2x = plist.at(plist.size()-2).toInt();
    int p2y = plist.last().toInt();

    WireLine* line1 = nullptr;

    for( int i=2; i<plist.size(); i+=2 )
    {
        p2x = plist.at(i).toInt();
        p2y = plist.at(i+1).toInt();

        WireLine* line2 = new WireLine( p1x, p1y, p2x, p2y, this );
        line2->setIsBus( m_wireFlags & wireBus );
        m_wireLineList.insert( index, line2 );
        Circuit::self()->addItem( line2 );

        if( line1 )
        {
            line1->setNextLine( line2 );
            line2->setPrevLine( line1 );
        }
        index++;
        line1 = line2;
        p1x = p2x;
        p1y = p2y;
}   }

void Wire::refreshPointList()
{
    if( m_wireLineList.isEmpty() ) return;

    QStringList list;
    list.append( QString::number( m_wireLineList.at(0)->p1().x() ) );
    list.append( QString::number( m_wireLineList.at(0)->p1().y() ) );

    for( int i=0; i<m_wireLineList.size(); i++ )
    {
        list.append( QString::number( m_wireLineList.at(i)->p2().x() ) );
        list.append( QString::number( m_wireLineList.at(i)->p2().y() ) );
    }
    m_pointList = list;
}

void Wire::addConLine( WireLine* line, int index )
{
    if( index > 0  && index < m_wireLineList.size() ) disconnectLines( index-1, index );

    m_wireLineList.insert( index, line );
    Circuit::self()->addItem( line );

    if( index > 0 )
    {
        connectLines( index-1, index );
        m_wireLineList.at( index-1 )->sSetP2( line->p1() );
    }
    if( index < m_wireLineList.size()-1 )
    {
        if( m_wireLineList.size() < 2 ) return;

        connectLines( index, index+1 );
        m_wireLineList.at( index+1 )->sSetP1( line->p2() );
    }
    line->setIsBus( m_wireFlags & wireBus );
    if( Circuit::self()->getNewWire() ) line->setCursor( Qt::ArrowCursor );
}

WireLine* Wire::addConLine( int x1, int y1, int x2, int y2, int index )
{
    WireLine* line = new WireLine( x1, y1, x2, y2, this );
    addConLine( line, index );
    return line;
}

void Wire::connectLines( int index1, int index2 )
{
    if( index1 < 0 || index2 < 0 || index2 > m_wireLineList.length()-1 )
        return;

    WireLine* line1 = m_wireLineList.at( index1 );
    WireLine* line2 = m_wireLineList.at( index2 );

    line1->setNextLine( line2 );
    line2->setPrevLine( line1 );
}

void Wire::disconnectLines( int index1, int index2 )
{
    if( index1 < 0 || index2 < 0 || index2 > m_wireLineList.length()-1 )
        return;

    m_wireLineList.at( index1 )->setNextLine( nullptr );
    m_wireLineList.at( index2 )->setPrevLine( nullptr );
}

void Wire::updateConRoute( QPointF thisPoint )
{
    updateConRoute( nullptr, thisPoint );
}

void Wire::updateConRoute( PinBase* pin )
{
    QPointF thisPoint = pin->scenePos();
    updateConRoute( pin, thisPoint );
}

void Wire::updateConRoute( PinBase* pin, QPointF thisPoint )
{
    if( m_wireLineList.isEmpty() ) return;
    if( !m_wireLineList.first()->isVisible() ) return;
    if( Circuit::self()->pasting() )  { remNullLines(); return; }

    bool diagonal = false;
    int length = m_wireLineList.length();
    WireLine* line;
    WireLine* preline = nullptr;

    if( pin == m_startPin )
    {
        line = m_wireLineList.first();
        diagonal = line->isDiagonal();
        line->sSetP1( thisPoint.toPoint() );
        m_lastIndex = 0;

        if( length > 1 ){
            preline = m_wireLineList.at(1);
            m_actLine = 1;
        }
        else m_actLine = 0;
        
        if( diagonal ) { remNullLines(); return; }
    }else{
        line = m_wireLineList.last();
        diagonal = line->isDiagonal();
        
        line->sSetP2( toGrid( thisPoint ).toPoint() );
        
        m_lastIndex = length-1;

        if( length > 1 ){
            preline = m_wireLineList.at( m_lastIndex-1 );
            if( pin != nullptr ) m_actLine = m_lastIndex-1;
        }
        if( diagonal || m_freeLine ) 
        {
            m_freeLine = false;
            if( m_lastIndex == m_actLine )          // Add new corner
            {
                QPoint point = line->p2();
                WireLine* newLine = addConLine( point.x(), point.y(), point.x()+4, point.y()+4, m_lastIndex + 1 );
                if( line->isSelected() ) newLine->setSelected( true );
            }
            remNullLines();
            return;
    }   }
    if( (line->dx() == 0) && (line->dy() == 0) && (length > 1) ) // Null Line
    {
        if( line->scene() ) Circuit::self()->removeItem( line );
        m_wireLineList.removeOne( line );

        if( m_actLine > 0 )  m_actLine -= 1;
    }
    else if( line->dx() != 0 && line->dy() != 0 )
    {
        QPoint point;

        if( m_lastIndex == m_actLine )          // Add new corner
        {
            point = line->p2();

            if( fabs(line->dx()) > fabs(line->dy()) ) point.setY( line->p1().y() );
            else                                      point.setX( line->p1().x() );

            WireLine* newLine = addConLine( point.x(), point.y(), line->p2().x(), line->p2().y(), m_lastIndex + 1 );

            if( line->isSelected() ) newLine->setSelected( true );
            line->setP2( point );
        }
        else if( m_lastIndex < m_actLine )        // Update first corner
        {
            point = line->p2();

            if( preline->dx() == 0 ) point.setY( line->p1().y() );
            else                     point.setX( line->p1().x() );

            line->setP2( point );

            if( line->dx() == preline->dx() || line->dy() == preline->dy() ) // Lines aligned or null line
            {
                if( line->isSelected() || preline->isSelected() )
                {
                    preline->sSetP1( line->p1() );
                    remConLine( line  );
        }   }   }
        else{                                      // Update last corner
            point = line->p1();

            if( preline->dx() == 0 ) point.setY( line->p2().y() );
            else                     point.setX( line->p2().x() );

            line->setP1( point );

            if( line->dx() == preline->dx() || line->dy() == preline->dy() ) // Lines aligned or null line
            {
                if( line->isSelected() || preline->isSelected() )
                {
                    preline->sSetP2( line->p2() );
                    remConLine( line  );
    }   }   }   }
    remNullLines();
}

void Wire::remLines()
{
    while( !m_wireLineList.isEmpty() )
    {
        WireLine* line = m_wireLineList.takeLast();
        if( line->scene() ) Circuit::self()->removeItem( line );
        delete line;
}   }

void Wire::move( QPointF delta )
{
    if( !Circuit::self()->pasting() ) return;

    for( WireLine* line : m_wireLineList ) line->move( delta );
    m_startPin->isMoved();
    m_endPin->isMoved();
}

void Wire::select(  bool selected )
{ for( WireLine* line : m_wireLineList ) line->setSelected( selected ); }

void Wire::setVisib(  bool vis )
{ for( WireLine* line : m_wireLineList ) line->setVisible( vis ); }

void Wire::remove()
{
    if( Simulator::self()->isRunning() )  CircuitWidget::self()->powerCircOff();

    if( m_startPin ) m_startPin->wireRemoved( this );
    if( m_endPin )   m_endPin->wireRemoved( this );
    remLines();
}

void Wire::closeCon( PinBase* endpin )
{
    if( Simulator::self()->isRunning() ) CircuitWidget::self()->powerCircOff();

    m_endPin = endpin;
    m_startPin->setWire( this );
    m_endPin->setWire( this );
    m_startPin->setConPin( m_endPin );
    m_endPin->setConPin( m_startPin );
    m_startPin->setWireFlags( m_wireFlags );
    m_startPin->setWireFlags( m_wireFlags ); // Pin will set conPin isBus

    updateConRoute( m_endPin );

    for( WireLine* line : m_wireLineList ) line->setCursor( Qt::CrossCursor );
}

void Wire::splitCon( int index, PinBase* pin0, PinBase* pin2 )
{
    if( !m_endPin ) return;

    Wire* wire0 = new Wire( Circuit::self()->newWireId(), m_startPin );
    Circuit::self()->wireList()->append( wire0 );

    Wire* wire1 = new Wire( Circuit::self()->newWireId(), pin2 );
    Circuit::self()->wireList()->append( wire1 );

    disconnectLines( index-1, index );
    Wire* wire = wire0;
    int newindex = 0;
    int size = m_wireLineList.size();
    for( int i=0; i<size; ++i )
    {
        if( i == index ){ wire = wire1; newindex = 0; }
        WireLine* line = m_wireLineList.takeFirst();
        wire->lineList()->insert( newindex, line );
        line->setConnector( wire );
        if( newindex > 1 ) wire->incActLine();
        newindex++;
    }
    wire1->closeCon( m_endPin );    // Close con1 first please
    wire0->closeCon( pin0 );        // con0

    m_startPin = nullptr;
    m_endPin   = nullptr;
    Circuit::self()->removeWire( this );
}

void Wire::updateLines()
{
    for( WireLine* line : m_wireLineList ) line->animateLine();

    /*eNode* enode = startPin()->getEnode();
    if( enode && enode->voltchanged() )
    {
        enode->setVoltChanged( false );
        for( WireLine* line : m_wireLineList ) line->update();
    }*/
}

double Wire::getVoltage()
{
    if( m_wireFlags & wireFunc ) return 0;
    Pin* pin = (Pin*) m_startPin;
    return pin->getVoltage();
}
