/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "meter.h"

class VoltMeter : public Meter
{
    public:
        VoltMeter( int id );
        ~VoltMeter();

 static listItem_t registerItem();

        void updateStep() override;
};
