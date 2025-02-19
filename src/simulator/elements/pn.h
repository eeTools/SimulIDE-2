/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "resistance.h"

class PN : public Resistance
{
    public:
        PN();
        ~PN();

        virtual void stampAdmit() override;
        virtual void voltChanged() override;


    protected:

};
