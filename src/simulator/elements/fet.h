/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "resistance.h"

class Fet : public Resistance
{
    public:
        Fet();
        ~Fet();

        virtual void stampAdmit() override;
        virtual void voltChanged() override;

    protected:

        bool m_Pchannel;
        bool m_depletion;
};
