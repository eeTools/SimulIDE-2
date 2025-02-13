/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "portmodule.h"
#include "modsignal.h"

class mHookPort : public PortModule
{
    public:
        mHookPort( int id );
        ~mHookPort();

 static listItem_t registerItem();

        void setup() override;

        void runStep() override;

    protected:
        PinBase* addPin( QString id ) override;
};
