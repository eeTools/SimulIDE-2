/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef IOPIN_H
#define IOPIN_H

#include <QColor>

#include "pin.h"
#include "pinsource.h"

enum pinMode_t{
    undef_mode=0,
    input,
    openCo,
    output,
    source
};

class eNode;
class asIScriptEngine;

class IoPin : public Pin, public PinSource
{
        friend class Function;

    public:
        IoPin( int angle, const QPoint pos, QString id, Component* parent, pinMode_t mode=source );
        ~IoPin();

        void stampAdmit() override;
        void updateStep() override;
        /// void runEvent() override;

        virtual void scheduleState( bool state, uint64_t time );

        //pinMode_t pinMode() { return m_pinMode; }
        void setPinMode( pinMode_t mode );
        void setPinMode( uint mode ) { setPinMode( (pinMode_t) mode ); }

        void setInputHighV( double volt ) { m_inpHighV = volt; }
        void setInputLowV( double volt )  { m_inpLowV  = volt; }
        void setInputImp( double imp );
        void setInputAdmit( double a ) { m_admitance = a; }

        double outHighV() { return m_outHighV; }
        void  setOutHighV( double v ) { m_outHighV = v; }

        double outLowV() { return m_outLowV; }
        void  setOutLowV( double v ) { m_outLowV = v; }

        void startLH();
        void startHL();

        virtual void setOutputImp( double imp );
        virtual void setImpedance( double imp );

        virtual bool getInpState();
        virtual bool getOutState() { if( m_step ) return m_nextState; return m_outState; }
        virtual void setOutState( bool high );
        virtual void toggleOutState( uint64_t time=0 ) { scheduleState( !m_outState, time ); }

        double getVoltage() override;
        inline void setVoltage( double volt )
        {
            if( m_voltage == volt ) return;
            updtVoltage( volt );
        }
        inline void setOutStatFast( bool state )
        {
            m_outState = m_nextState = state;
            double voltage = state ? m_outHighV : m_outLowV;
            updtVoltage( voltage );
        }

        void setStateZ( bool z );
        virtual void setPullup( bool up );


        void setInverted( bool invert ); // override;

        virtual void controlPin( bool outCtrl , bool dirCtrl ){;}

        void skipStamp( bool s ) { m_skipStamp = s; }

        void setRiseTime( uint64_t time ) { m_timeRis = time; }
        void setFallTime( uint64_t time ) { m_timeFal = time; }

 static QStringList registerScript( asIScriptEngine* engine );

    protected:
        void contextMenuEvent( QGraphicsSceneContextMenuEvent* event ) override;

        void userInvertPin();

        inline void updtState()
        {
            double vddAdmit = m_vddAdmit + m_vddAdmEx;
            double gndAdmit = m_gndAdmit + m_gndAdmEx;
            updtAdmitance( vddAdmit+gndAdmit );

            m_voltage = m_outHighV*vddAdmit/m_admitance;
            if( m_node >= 0 ) updateCurrent();
        }

        double m_inpHighV;  // currently in eClockedDevice
        double m_inpLowV;

        double m_outHighV;
        double m_outLowV;

        double m_vddAdmit;  // Out stage
        double m_gndAdmit;  // Out Stage
        double m_vddAdmEx;  // Extra Source (vref out)
        double m_gndAdmEx;  // Extra Source (vref out)

        double m_inputImp;
        double m_outputImp;
        double m_openImp;

        bool m_inpState;
        bool m_outState;
        bool m_stateZ;
        bool m_nextState;
        bool m_skipStamp;
        bool m_userInvert;

        int m_steps;
        int m_step;

        uint64_t m_timeRis;  // Time for Output voltage to switch from 0% to 100%
        uint64_t m_timeFal;  // Time for Output voltage to switch from 100% to 0%

        pinMode_t m_pinMode;
};
#endif
