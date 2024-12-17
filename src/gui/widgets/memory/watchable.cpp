/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "watchable.h"
//#include "monitor.h"
#include "watcher.h"

Watchable::Watchable()
{
    //m_monitor = nullptr;
    m_watcher = nullptr;
}
Watchable::~Watchable()
{
    //if( m_monitor ) delete m_monitor;
}

/*Monitor* Watchable::getMonitor()
{
    if( !m_monitor )
    {
        m_monitor = new Monitor();
    }
    //m_monitor->setWindowTitle( idLabel() );
    m_monitor->show();

    return m_monitor;
}*/

Watcher* Watchable::getWatcher()
{
    if( !m_watcher ) m_watcher = new Watcher( nullptr, this );
    return m_watcher;
}

/*void Watchable::createWatcher( Watchable* cpu )
{

}*/
