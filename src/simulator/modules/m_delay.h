/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef DELAY_H
#define DELAY_H

#include "module.h"
#include "modsignal.h"
#include "callback.h"

class Delay : public Module
{
    public:
        Delay( QString name );
        ~Delay();

 static moduleItem_t registerItem();

        double delayNanoS() { return m_delay/1e12; }
        void setDelayNanoS( double t ) {
            m_delay = t*1e12;
        }

        virtual void initModule() override;
        virtual void runStep() override;

        void runOutput( SimEventInt<Delay>* event );
        //void setDelay( int s );

    private:
        int m_outState;

        uint64_t m_delay;  // dealty time in ps

        SimEventInt<Delay>* m_freeEvent;

        ModSlot   m_inputSlot;
        ModSignal m_outSignal;

 static QString m_moduleType;
};
#endif
