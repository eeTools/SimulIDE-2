/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "module.h"
#include "modsignal.h"

enum clkState_t{
    Clock_Low = 0,
    Clock_Rising=1,
    Clock_Allow=1,
    Clock_High,
    Clock_Falling,
};

enum trigger_t {
    None = 0,
    Clock,
    InEnable
};

class IoPin;

class Clocked : public Module
{
    public:
        Clocked( int id );
        ~Clocked();

        /// virtual void stamp() override;

        trigger_t trigger() { return m_trigger; }
        virtual void setTrigger( trigger_t trigger );

        QString triggerStr() { return m_triggerStr.at( (int)m_trigger ); }
        void setTriggerStr( QString t ){ setTrigger( (trigger_t)m_triggerStr.indexOf(t) );}

        void setClockPin( IoPin* clockPin ) { m_clkPin = clockPin; }

        ///virtual QStringList getEnumUids( QString p ) override;
        ///virtual QStringList getEnumNames( QString p ) override;

        //bool clockInv();
        //void setClockInv( bool inv );

        void updateClock();

        void remove();

    protected:
        bool m_clock;

        clkState_t m_clkState;
        trigger_t  m_trigger;

        IoPin* m_clkPin;

        ModSlot m_clockSlot;

 static QStringList m_triggerStr;
};
