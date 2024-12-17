/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MONITOR_H
#define MONITOR_H

//#include <QDialog>
//#include <QTableWidget>

#include "ui_monitor.h"

class MemTable;
class Watcher;

class Monitor : public QDialog, private Ui::Monitor
{
    Q_OBJECT
    
    public:
        Monitor( QWidget* parent=nullptr );

        void updateStep();

        void addTable( MemTable* table, QString tittle );
        void addWatcher( Watcher* watcher );

    public slots:
        void tabChanged(int);

    private:

        Watcher* m_watcher;
};

#endif
