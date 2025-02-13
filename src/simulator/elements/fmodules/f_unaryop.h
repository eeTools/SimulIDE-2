/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "fmodule.h"
//#include "clocked.h"

class fUnaryOp : public fModule //public Clocked
{
    public:
        fUnaryOp();
        ~fUnaryOp();

        enum unOpType_t{
            AND=0,
            OR,
            XOR,
            NOT,
            INC,    // Increase
            DEC,    // Decrease
            RTL,    // Rotate Left
            RTR,    // Rotate Right
            DTB,    // DEC to BCD (Encoder)
            BTD     // BCD to DEC (Decoder)
        };

        void initModule() override;
        void updateOutput() override;
        void connectSignal( fModule* signalModule, int slotId ) override;

        int size() { return m_bits; }
        void setSize( int s );

    private:


        inline int dtb();

        unOpType_t  m_unOpType;

        int m_bits;
        int m_mask;
        int* m_input;
};
