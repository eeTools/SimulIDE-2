/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "kcl.h"

enum simState_t{
    SIM_STOPPED=0,
    SIM_ERROR,
    SIM_STARTING,
    SIM_PAUSED,
    SIM_WAITING,
    SIM_RUNNING,
    SIM_DEBUG,
};

#include <QElapsedTimer>
#include <QFuture>

class Updatable;
class Component;
class Element;
class CallBack;

class Simulator : public QObject, public Kcl
{
        friend class eNode;

    Q_OBJECT
    public:
        Simulator( QObject* parent=0 );
        ~Simulator();

 static Simulator* self() { return m_pSelf; }

         void addEvent( uint64_t time, CallBack* cb );
         //void cancelEvents( eElement* el );

        void startSim( bool paused=false );
        void pauseSim();
        void resumeSim();
        void stopSim();

        void setWarning( int warning ) { m_warning = warning; }
        
        uint64_t fps() { return m_fps; }
        void setFps( uint64_t fps );
        uint64_t psPerFrame() { return m_psPF; }
        uint64_t realPsPF() { return m_realPsPF; }

        uint64_t psPerSec() { return m_psPerSec; } // Speed picosecond/second
        void setPsPerSec( uint64_t psPs );

        uint64_t stepSize() { return m_stepSize; }
        void setStepSize( uint64_t stepSize ) { m_stepSize = stepSize; }

        uint64_t stepsPerSec() { return m_stepsPS; }
        void setStepsPerSec( uint64_t sps );

        uint64_t reactStep() { return m_reactStep; }
        void setReactStep( uint64_t rs ) { m_reactStep = rs; }

        void  setSlopeSteps( int steps ) { m_slopeSteps = steps; }
        int slopeSteps( ) { return m_slopeSteps; }

        void  setMaxNlSteps( uint32_t steps ) { m_maxNlStep = steps; }
        uint32_t maxNlSteps( ) { return m_maxNlStep; }
        
        bool isRunning() { return (m_state >= SIM_STARTING); }
        bool isPaused()  { return (m_state == SIM_PAUSED); }
        bool isPauseDebug() { return (m_state == SIM_PAUSED && m_debug == true); }

        uint64_t circTime() { return m_simTime; }

        void timerEvent( QTimerEvent* e );

        double realSpeed() { return m_realSpeed; } // 0 to 10000 => 0 to 100%

        simState_t simState() { return m_state; }

        inline void notCorverged() { m_converged = false; }

        void addElement( Element* e ) { m_elements.append(e); }
        void remElement( Element* e ) { m_elements.removeAll(e); }
        
        void addToUpdateList( Updatable* el );
        void remFromUpdateList( Updatable* el );

        void addToComponentList( Component* el );
        void remFromComponentList( Component* el );

        void addToPinList( QString pin ) { if( !m_pinList.contains(pin) ) m_pinList.append(pin); }

    private:
 static Simulator* m_pSelf;

        void createNodes();
        void resetSim();
        void runCircuit();
        inline void solveCircuit();

        inline void clearEventList();

        //inline void stopTimer();
        //inline void initTimer();

        //eElement* m_firstEvent;
        CallBack* m_firstEvent;

        QList<Element*> m_elements;
        QStringList m_pinList; // used in createNodes()

        QFuture<void> m_CircuitFuture;

        QHash<int, QString> m_errors;
        QHash<int, QString> m_warnings;

        QList<Updatable*> m_updateList;
        QList<Component*> m_componentList;

        simState_t m_state;
        simState_t m_oldState;

        bool m_running;

        bool m_debug;
        bool m_converged;
        bool m_pauseCirc;

        int m_error;
        int m_warning;
        int m_timerId;
        int m_timer_ms;
        int m_slopeSteps;

        uint64_t m_fps;
        uint32_t m_NLstep;
        uint32_t m_maxNlStep;

        uint64_t m_reactStep;
        uint64_t m_psPerSec;
        uint64_t m_stepSize;  ///
        uint64_t m_stepsPS;   ///
        uint64_t m_psPF;
        uint64_t m_realPsPF;
        double   m_realSpeed;

        uint64_t m_simTime;
        uint64_t m_tStep;
        uint64_t m_lastStep;
        uint64_t m_refTime;
        uint64_t m_lastRefT;
        uint64_t m_loopTime;
        uint64_t m_guiTime;
        uint64_t m_updtTime;
        double   m_simLoad;

        QElapsedTimer m_RefTimer;
};
 #endif


