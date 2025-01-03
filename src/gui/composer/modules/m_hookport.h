/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef HOOKPORT_H
#define HOOKPORT_H

#include "portmodule.h"
#include "modsignal.h"

class mHookPort : public PortModule
{
    public:
        mHookPort( QString name );
        ~mHookPort();

 static listItem_t registerItem();

        void setup() override;

        void runStep() override;

    protected:
        PinBase* addPin( QString id ) override;
};
#endif
