/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef WATCHABLE_H
#define WATCHABLE_H

#include <QString>

class Watcher;

class Watchable
{
    public:
        Watchable();
        ~Watchable();

        //Monitor* getMonitor();

        virtual Watcher* getWatcher();
        //void createWatcher( Watchable* cpu );

        virtual int getIntReg( QString ){ return -1;}
        virtual QString getStrReg( QString ){ return "";}

        virtual void command( QString ){;}

    protected:

        //Monitor* m_monitor;
        Watcher* m_watcher;
};

#endif
