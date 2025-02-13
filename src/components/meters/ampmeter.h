/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "meter.h"

class AmpMeter : public Meter
{
    public:
        AmpMeter( int id );
        ~AmpMeter();

 static listItem_t registerItem();

        void updateStep() override;
};
