/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef IOPIN_H
#define IOPIN_H

//#include <QColor>

#include "pin.h"
#include "pinsource.h"
#include "callback.h"

enum pinDirection_t{
    pinInput=0,
    pinOutput
};

class eNode;
class asIScriptEngine;

class IoPin : public Pin
{
    public:
        IoPin( int angle, const QPoint pos, QString id, Component* parent, bool pullup=false );
        ~IoPin();

        /// void runEvent() override;



        void setInputHighV( double volt ) { m_inpHiVolt = volt; }
        void setInputLowV( double volt )  { m_inpLoVolt  = volt; }
        void setInputImp( double r ) { m_inpSource.updtAdmitance( 1/r ); }
        bool getLogicState();

        //double outHighV() { return m_outHiVolt; }
        void  setOutHighV( double v ) { m_outHiVolt = v; }

        //double outLowV() { return m_outLoVolt; }
        void setOutLowV( double v ) { m_outLoVolt = v; }

        void setOutputImp( double r ){ m_outSource.updtAdmitance( 1/r ); }

        virtual void setVoltage( double v ) { m_voltage = v; }

        virtual bool getOutState() { if( m_step ) return m_nextState; return m_outState; }
        virtual void setOutState( bool high ){;}
        virtual void scheduleState( bool state, uint64_t time );
        //virtual void toggleOutState( uint64_t time=0 ) { scheduleState( !m_outState, time ); }

        void enableOutput( bool e ){;}

        virtual void setDirection( pinDirection_t d ){ m_direction = d;}

        void setRiseTime( uint64_t time ) { m_timeRis = time; }
        void setFallTime( uint64_t time ) { m_timeFal = time; }

        void setPullup( double r ) { m_pulSource.updtAdmitance( 1/r ); }

        void setInverted( bool invert ) override;

 static QStringList registerScript( asIScriptEngine* engine );

    protected:
        void contextMenuEvent( QGraphicsSceneContextMenuEvent* event ) override;

        void userInvertPin() { setInverted( !m_inverted ); }

        double m_inpHiVolt;
        double m_inpLoVolt;

        double m_outHiVolt;
        double m_outLoVolt;

        double m_inputAdmit;
        double m_outputAdmit;
        double m_pullupAdmit;

        bool m_logicState;
        bool m_outState;
        bool m_nextState;

        int m_steps;
        int m_step;

        pinDirection_t m_direction;

        uint64_t m_timeRis;  // Time for Output voltage to switch from 0% to 100%
        uint64_t m_timeFal;  // Time for Output voltage to switch from 100% to 0%

        PinSource m_outSource;
        PinSource m_inpSource;
        PinSource m_pulSource;
};

//--------------------------------------------------------
//--------------------------------------------------------

class InputPin : public IoPin
{
    public:
        InputPin( int angle, const QPoint pos, QString id, Component* parent, bool pullup=false );
        ~InputPin();

        void updateStep() override;

 static pinAnim_t getPinAnim( IoPin* pin );
};

//--------------------------------------------------------
//--------------------------------------------------------

class OutputPin : public IoPin
{
    public:
        OutputPin( int angle, const QPoint pos, QString id, Component* parent, bool pullup=false );
        ~OutputPin();

        void updateStep() override;
        void runStep();

        void setOutState( bool state ) override
        {
            if( m_outState == state ) return;
            m_outState = state;
            if( m_inverted ) state = !state;

            double voltage = state ? m_outHiVolt : m_outLoVolt;
            setVoltage( voltage );
        }

        inline void setVoltage( double v ) override
        {
            if( m_voltage == v ) return;
            m_voltage = v;
            m_outSource.updateVoltage( m_voltage );
        }

 static pinAnim_t getPinAnim( IoPin* pin );

    protected:

        SimEvent<OutputPin> m_slopeEvent; // Callback for simulation event
};

//--------------------------------------------------------
//--------------------------------------------------------

class BidirPin : public OutputPin
{
    public:
        BidirPin( int angle, const QPoint pos, QString id, Component* parent, bool pullup=false );
        ~BidirPin();

        void updateStep() override;

        void setDirection( pinDirection_t d ) override;

        inline void setOutState( bool state ) override
        {
            if( m_direction == pinInput ) m_nextState = state;
            else                          OutputPin::setOutState( state );
        }
};

//--------------------------------------------------------
//--------------------------------------------------------

class OpenColPin : public IoPin
{
    public:
        OpenColPin( int angle, const QPoint pos, QString id, Component* parent, bool pullup=false );
        ~OpenColPin();

        void updateStep() override;
        void runStep();

        void setOutState( bool high ) override;

    protected:

        SimEvent<OpenColPin> m_slopeEvent; // Callback for simulation event
};

#endif
