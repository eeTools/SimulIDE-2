/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef INTTOBIT_H
#define INTTOBIT_H

#include "module.h"
#include "modsignal.h"

class IntToBit : public Module
{
    public:
        IntToBit( QString name );
        ~IntToBit();

 static listItem_t registerItem();

        void initModule() override;
        void runStep() override;

        int size() { return m_bits; }
        void setSize( int s );

    private:

        int m_bits;
        int m_mask;
        int m_input;

        std::vector<int> m_outputs;

        ModSlot m_inputSlot;
};
#endif
