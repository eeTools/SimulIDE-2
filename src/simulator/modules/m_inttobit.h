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

        virtual void initModule() override;
        virtual void runStep() override;

        int size() { return m_bits; }
        void setSize( int s );

    private:

        int m_bits;
        int m_mask;
        int m_input;

        ModSlot m_inputSlot;

 static QString m_moduleType;
};
#endif
