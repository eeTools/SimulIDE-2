/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "module.h"
#include "modsignal.h"
#include "callback.h"

class Delay : public Module
{
    public:
        Delay( int id );
        ~Delay();

 static listItem_t registerItem();

        double delayNanoS() { return m_delay/1e12; }
        void setDelayNanoS( double t ) {
            m_delay = t*1e12;
        }

        void initModule() override;
        void runStep() override;

        void runOutput( SimEventInt<Delay>* event );
        //void setDelay( int s );

    private:
        int m_outState;

        uint64_t m_delay;  // dealty time in ps

        SimEventInt<Delay>* m_freeEvent;

        ModSlot   m_inputSlot;
        ModSignal m_outSignal;
};
