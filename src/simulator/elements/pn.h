/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef PN_H
#define PN_H

#include "resistance.h"

class PN : public Resistance
{
    public:
        PN( QString id );
        ~PN();

        virtual void stampAdmit() override;
        virtual void voltChanged() override;


    protected:

};

#endif
