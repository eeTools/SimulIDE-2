/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "fmodule.h"

class fBinaryOp : public fModule
{
    public:
        fBinaryOp();
        ~fBinaryOp();

        enum binOpType_t{
            AND=0,
            OR,
            XOR,
            ADD,    // A + B
            SUB,    // A - B
            MUL,    // A * B
            DIV,    // A / B
            MOD,    // A % B
            CMP     // A <=> B
        };

        void initModule() override;
        void updateOutput() override;
        void connectSignal( fModule* signalModule, int slotId ) override;

        int size() { return m_bits; }
        void setSize( int s );

    private:

        inline int cmp();

        binOpType_t m_binOpType;

        int m_bits;
        int m_mask;
        int* m_inputA;
        int* m_inputB;
};
