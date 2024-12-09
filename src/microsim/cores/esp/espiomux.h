/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef ESPIOMUX_H
#define ESPIOMUX_H

#include "mcumodule.h"

class EspIoMux : public McuModule
{
    friend class McuPort;

    public:
        EspIoMux( eMcu* mcu, QString name );
        ~EspIoMux();

    protected:

};

#endif
