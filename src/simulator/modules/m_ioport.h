/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef MIOPORT_H
#define MIOPORT_H

#include "portbase.h"
#include "modsignal.h"

//class IoPin;

class mIoPort : public PortBase
{
    public:
        mIoPort( QString name );
        virtual ~mIoPort();

 static listItem_t registerItem();

        virtual void setup() override;

        virtual void initModule() override;
        virtual void runStep() override;

    protected:
        PinBase* addPin( QString id ) override;

        int m_state;

        ModSlot   m_inputSlot;
        ModSignal m_outSignal;

 static QString m_moduleType;
};
#endif
