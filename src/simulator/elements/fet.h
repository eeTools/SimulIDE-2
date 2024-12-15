/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef FET_H
#define FET_H

#include "resistance.h"

class Fet : public Resistance
{
    public:
        Fet( QString id );
        ~Fet();

        virtual void stampAdmit() override;
        virtual void voltChanged() override;

        
    protected:

        bool m_Pchannel;
        bool m_depletion;

};

#endif
