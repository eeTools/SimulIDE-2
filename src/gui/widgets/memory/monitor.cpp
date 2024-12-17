/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "monitor.h"
#include "simulator.h"
#include "memtable.h"
#include "watcher.h"

Monitor::Monitor( QWidget* parent )
       : QDialog( parent )
{
    setupUi(this);
    hide();

    m_watcher = nullptr;

    connect( tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)) );
}

void Monitor::addTable( MemTable* table, QString tittle )
{
    tabWidget->addTab( table, tittle );
}

void Monitor::addWatcher( Watcher* watcher )
{
    m_watcher = watcher;
    if( m_watcher ) tabWidget->addTab( m_watcher, tr("Watch") );
}

void Monitor::tabChanged( int )
{
    if( Simulator::self()->isRunning() ) return;
    updateStep();
}

void Monitor::updateStep()
{
    if( m_watcher && m_watcher->isVisible() )
    {
        m_watcher->updateValues();
    }
}
