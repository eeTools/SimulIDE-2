/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef ABOP_H
#define ABOP_H

#include "module.h"
#include "modsignal.h"

class AbOp : public Module
{
    public:
        AbOp( QString name );
        ~AbOp();

        enum abOpType{
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

        virtual void initModule() override;
        virtual void runStep() override;

        int size() { return m_bits; }
        void setSize( int s );

        QString typeStr() { return m_abOpStr; }
        void setTypeStr( QString type );

    private:
        void updateOutput();

        inline int cmp();

        abOpType m_abOpType;
        QString m_abOpStr;
        QStringList m_abOpList;

        int m_bits;
        int m_mask;
        int m_inputA;
        int m_inputB;
        int m_output;

        ModSlot   m_inSlotA;
        ModSlot   m_inSlotB;
        ModSignal m_outSignal;

 static QString m_moduleType;
};
#endif
