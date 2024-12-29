/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef BINARYOP_H
#define BINARYOP_H

#include "module.h"
#include "modsignal.h"

class BinaryOp : public Module
{
    public:
        BinaryOp( QString name );
        ~BinaryOp();

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

 static listItem_t registerItem();

        void initModule() override;
        void runStep() override;

        int size() { return m_bits; }
        void setSize( int s );

        QString typeStr() { return m_binOpStr; }
        void setTypeStr( QString type );

    private:
        void updateOutput();

        inline int cmp();

        binOpType_t m_binOpType;
        QString     m_binOpStr;
        QStringList m_binOpList;

        int m_bits;
        int m_mask;
        int m_inputA;
        int m_inputB;
        int m_output;

        ModSlot   m_inSlotA;
        ModSlot   m_inSlotB;
        ModSignal m_outSignal;
};
#endif
