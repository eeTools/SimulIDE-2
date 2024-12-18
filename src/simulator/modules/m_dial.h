/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef MDIAL_H
#define MDIAL_H

#include "module.h"
#include "modsignal.h"

class mDial : public Module
{
    public:
        mDial( QString name );
        ~mDial();

 static listItem_t registerItem();

        virtual void initModule() override;
        virtual void runStep() override;

    private:

        int m_output;

        ModSignal m_outSignal;
};
#endif
