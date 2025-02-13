/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "module.h"
#include "modsignal.h"

class WaveGen : public Module
{
    public:
        WaveGen( int id );
        ~WaveGen();

        enum waveType{
            SINE=0,
            SAW,
            TRIANGLE,
            SQARE,
            RANDOM,
            WAV
        };

 static listItem_t registerItem();

        void initModule() override;
        void runStep() override;

    private:

        double m_output;

        ModSignal m_outSignal;

        waveType m_waveType;
        QStringList m_waveList;
};
