/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef Inductance_H
#define Inductance_H

#include "resistance.h"

class Inductance : public Resistance
{
    public:
        Inductance( QString id );
        ~Inductance();

        virtual void stampAdmit() override;
        //virtual void stampCurrents() override;
        virtual void voltChanged() override;

    protected:

};

#endif
