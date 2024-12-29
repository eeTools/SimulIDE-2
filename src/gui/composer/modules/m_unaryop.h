/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef UNARYOP_H
#define UNARYOP_H

#include "modsignal.h"
#include "clocked.h"

class UnaryOp : public Clocked
{
    public:
        UnaryOp( QString name );
        ~UnaryOp();

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

 static listItem_t registerItem();

        void initModule() override;
        void runStep() override;

        //void setState( int s );
        //void setBitState( int s, int bit );

        int size() { return m_bits; }
        void setSize( int s );

        QString typeStr() { return m_unOpStr; }
        void setTypeStr( QString type );

    private:
        void updateOutput();

        inline int dtb();

        unOpType_t  m_unOpType;
        QString     m_unOpStr;
        QStringList m_unOpList;

        int m_bits;
        int m_mask;
        int m_input;
        int m_output;

        ModSlot   m_inputSlot;
        ModSignal m_outSignal;
};
#endif
