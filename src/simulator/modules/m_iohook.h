/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef IOHOOK_H
#define IOHOOK_H

#include "portbase.h"
#include "modsignal.h"

class IoHook : public PortBase
{
    public:
        IoHook( QString name );
        ~IoHook();

 static moduleItem_t registerItem();

        virtual void runStep() override;

    protected:
        PinBase* addPin( QString id ) override;

        int m_outState;

        ModSlot   m_inputSlot;
        ModSignal m_outSignal;

 static QString m_moduleType;
};
#endif
