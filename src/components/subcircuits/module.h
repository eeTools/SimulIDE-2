/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MODULE_H
#define MODULE_H

#include "board.h"

class ModuleSubc : public BoardSubc
{
    public:
        ModuleSubc( QString type, QString id );
        ~ModuleSubc();
        
        double zVal() { return zValue(); }
        void setZVal( double v);

        virtual void slotAttach();

    protected:
        virtual void renameTunnels();
};
#endif
