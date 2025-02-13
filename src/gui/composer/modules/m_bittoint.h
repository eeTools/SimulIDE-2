/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "module.h"
#include "modsignal.h"

class BitToInt : public Module
{
    public:
        BitToInt( int id );
        ~BitToInt();

 static listItem_t registerItem();

        virtual void initModule() override;
        virtual void runStep() override;

        int size() { return m_bits; }
        void setSize( int s );

    private:

        int m_bits;
        int m_output;

        ModSignal m_outSignal;
};
