/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef VOLTMETER_H
#define VOLTMETER_H

#include "meter.h"

class VoltMeter : public Meter
{
    public:
        VoltMeter( QString id );
        ~VoltMeter();

 static listItem_t registerItem();

        virtual void updateStep() override;
};

#endif
