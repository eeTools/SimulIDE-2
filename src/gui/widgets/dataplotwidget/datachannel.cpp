/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "datachannel.h"
#include "plotdisplay.h"
#include "plotbase.h"
#include "simulator.h"
#include "wire.h"
#include "pin.h"

DataChannel::DataChannel( PlotBase* plotBase, QString id )
           : Element()
           , Updatable()
{
    m_plotBase = plotBase;

    m_pin = nullptr;
    m_node = -1;

    m_refPin = nullptr;
    m_refNode = -1;

    m_chTunnel = "";
    m_trigIndex = 0;
    m_pauseOnCond = false;
}
DataChannel::~DataChannel(){}

void DataChannel::stampAdmit()    // Called at Simulation Start
{
    m_bufferCounter = 0;
    m_trigIndex = 0;
}

void DataChannel::stampCurrent()
{
    m_node = m_pin->getNode();
    if( m_refPin ) m_refNode = m_refPin->getNode();
    bool connected = m_node >= 0;

    if( connected ) m_kcl->addChangeCB( this, m_node );

    m_plotBase->display()->connectChannel( m_channel, connected );
}

bool DataChannel::isBus()
{
    if( m_pin ) return m_pin->wireFlags() & wireBus;
    return false;
}

bool DataChannel::doTest()
{
    int dataSize = m_bufferTest.size();
    bool compare = dataSize > 0;
    if( compare && dataSize != m_bufferCounter ) return false;

    bool ok = true;
    for( int n=0; n<m_bufferCounter; ++n )
    {
        uint64_t time = m_time.at(n);
        double  value = m_buffer.at(n);
        if( compare )
        {
            uint64_t cTime = m_timeTest.at(n);
            double  cValue = m_bufferTest.at(n);
            if( time != cTime || value != cValue ) ok = false;
        }else{
            m_timeTest.append( time );
            m_bufferTest.append( value );
        }
    }
    return ok;
}

QString DataChannel::testData()
{
    QString td;

    for( int n=0; n<m_bufferTest.size(); ++n )
    {
        td += QString::number( m_timeTest.at(n), 16 )+" ";
        td += QString::number( m_bufferTest.at(n) )+"|";
    }
    td.remove( td.size()-1, 1);
    return td;
}

void DataChannel::setTestData( QString td )
{
    if( td.isEmpty() ) return;

    QStringList dataList = td.split("|");
    for( int i=0; i<dataList.size(); ++i )
    {
        QStringList dataPair = dataList.at(i).split(" ");
        m_timeTest.append( dataPair.first().toUInt(0,16) );
        m_bufferTest.append( dataPair.last().toDouble() );
    }
}
