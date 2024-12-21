/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef WAVEGEN_H
#define WAVEGEN_H

#include "module.h"
#include "modsignal.h"

class WaveGen : public Module
{
    public:
        WaveGen( QString name );
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

        virtual void initModule() override;
        virtual void runStep() override;

    private:

        double m_output;

        ModSignal m_outSignal;

        waveType m_waveType;
        QStringList m_waveList;
};
#endif
