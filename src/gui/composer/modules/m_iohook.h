/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef IOHOOK_H
#define IOHOOK_H

#include "portbase.h"
#include "modsignal.h"

class IoHook : public PortBase
{
    public:
        IoHook( QString name );
        ~IoHook();

 static listItem_t registerItem();

        void runStep() override;

    protected:
        PinBase* addPin( QString id ) override;
};
#endif
