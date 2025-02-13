/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "module.h"
#include "modsignal.h"

class BinaryOp : public Module
{
    public:
        BinaryOp( int id );
        ~BinaryOp();

 static listItem_t registerItem();

        int size() { return m_bits; }
        void setSize( int s );

        QString typeStr() { return m_binOpStr; }
        void setTypeStr( QString type );

    private:

        //binOpType_t m_binOpType;
        QString     m_binOpStr;
        QStringList m_binOpList;

        int m_bits;

        ModSlot   m_inSlotA;
        ModSlot   m_inSlotB;
        ModSignal m_outSignal;
};
