/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef AMPERIMETER_H
#define AMPERIMETER_H

#include "meter.h"

class Amperimeter : public Meter
{
    public:
        Amperimeter( QString id );
        ~Amperimeter();

 static listItem_t libraryItem();

        virtual void updateStep() override;
};

#endif
