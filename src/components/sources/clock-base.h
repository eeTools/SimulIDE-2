/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef CLOCKBASE_H
#define CLOCKBASE_H

#include <QGraphicsProxyWidget>

#include "component.h"
#include "pinsource.h"
#include "callback.h"

class CustomButton;
class IoPin;

class ClockBase : public Component
{
    public:
        ClockBase( QString id );
        ~ClockBase();

        void initialize() override;
        void updateStep() override;

        virtual void runEvent(){;}

        bool alwaysOn() { return m_alwaysOn; }
        void setAlwaysOn( bool on );

        double freq() { return m_freq; }
        virtual void setFreq( double freq );

        bool running() { return m_isRunning; }
        void setRunning( bool running );

        double voltage() { return m_voltage; }
        void setVoltage( double v );

        void setOut( bool ) {;}

        void setLinkedValue( double v, int i=0 ) override;

        void setHidden( bool hid, bool hidArea=false, bool hidLabel=false ) override;

        void onbuttonclicked();

    protected:
        double m_voltage;

        bool m_state;
        bool m_isRunning;
        bool m_alwaysOn;

        //uint64_t m_lastTime;
        uint64_t m_stepsPC;
        double m_fstepsPC;

        double m_freq;
        double m_remainder;

        IoPin* m_outPin;

        SimEvent<ClockBase> m_event; // Callback for simulation event

        CustomButton* m_button;
        QGraphicsProxyWidget* m_proxy;
};

#endif
