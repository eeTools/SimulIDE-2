/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "module.h"
#include "modsignal.h"

class Pin;

class mCurrentSource : public Module
{
    public:
        mCurrentSource( int id );
        ~mCurrentSource();

 static listItem_t registerItem();

    void setComponent( fComponent* c ) override;

    void initModule() override;
    void runStep() override;

    double gain() { return m_gain; }
    void setGain( double g ) { m_gain = g; }

    private:

        Pin* m_pPin;
        Pin* m_nPin;

        double m_input;
        double m_current;
        double m_gain;

        ModSlot   m_inputSlot;
};
