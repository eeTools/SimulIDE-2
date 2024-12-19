/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef BITOP_H
#define BITOP_H

#include "modsignal.h"
#include "clocked.h"

class BitOp : public Clocked
{
    public:
        BitOp( QString name );
        ~BitOp();

        enum bitOpType{
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

 static listItem_t registerItem();

        virtual void initModule() override;
        virtual void runStep() override;

        //void setState( int s );
        //void setBitState( int s, int bit );

        int size() { return m_bits; }
        void setSize( int s );

        QString typeStr() { return m_bitOpStr; }
        void setTypeStr( QString type );

    private:
        void updateOutput();

        inline int dtb();

        bitOpType m_bitOpType;
        QString m_bitOpStr;
        QStringList m_bitOpList;

        int m_bits;
        int m_mask;
        int m_input;
        int m_output;

        ModSlot   m_inputSlot;
        ModSignal m_outSignal;
};
#endif
