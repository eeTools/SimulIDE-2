/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef AMPMETER_H
#define AMPMETER_H

#include "meter.h"

class AmpMeter : public Meter
{
    public:
        AmpMeter( QString id );
        ~AmpMeter();

 static listItem_t registerItem();

        void updateStep() override;
};

#endif
