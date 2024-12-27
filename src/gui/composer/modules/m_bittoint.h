/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef BITTOINT_H
#define BITTOINT_H

#include "module.h"
#include "modsignal.h"

class BitToInt : public Module
{
    public:
        BitToInt( QString name );
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
#endif
