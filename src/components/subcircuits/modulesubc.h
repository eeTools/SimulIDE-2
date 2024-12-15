/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MODULESUBC_H
#define MODULESUBC_H

#include "shield.h"

class ModuleSubc : public ShieldSubc
{
    public:
        ModuleSubc( QString id );
        ~ModuleSubc();
        
        double zVal() { return zValue(); }
        void setZVal( double v);

        virtual void slotAttach() override;

    protected:
        virtual void renameTunnels() override;
};
#endif
