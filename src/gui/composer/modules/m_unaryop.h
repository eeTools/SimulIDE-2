/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "modsignal.h"
#include "clocked.h"

class UnaryOp : public Clocked
{
    public:
        UnaryOp( int id );
        ~UnaryOp();

 static listItem_t registerItem();

        int size() { return m_bits; }
        void setSize( int s );

        QString typeStr() { return m_unOpStr; }
        void setTypeStr( QString type );

    private:
        QString     m_unOpStr;
        QStringList m_unOpList;

        int m_bits;

        ModSlot   m_inputSlot;
        ModSignal m_outSignal;
};
