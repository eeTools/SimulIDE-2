/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef MIOPORT_H
#define MIOPORT_H

#include "portbase.h"
#include "ioport.h"
#include "modsignal.h"

//class IoPin;

class mIoPort : public PortBase, public IoPort
{
    public:
        mIoPort( QString name );
        virtual ~mIoPort();

 static listItem_t registerItem();

        void setup() override;

        void initModule() override;
        void runStep() override;

    protected:
        PinBase* addPin( QString id ) override;

        int m_state;

        ModSlot   m_inputSlot;
        ModSignal m_outSignal;
};
#endif
