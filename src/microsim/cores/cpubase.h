/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef CPUCORE_H
#define CPUCORE_H

#include "e_mcu.h"
//#include "mcuram.h"
//#include "mcupgm.h"
#include "watchable.h"

#define REG_SPL      m_spl[0]
#define REG_SPH      m_sph[0]
#define STATUS(bit) (*m_STATUS & (1<<bit))

class Display;
class Watcher;
class McuMmu;

class CpuBase : public Watchable
{
        friend class McuCreator;

    public:
        CpuBase( eMcu* mcu );
        virtual ~CpuBase();

        virtual void reset();
        virtual void runStep(){;}
        virtual void extClock( bool clkState ){;}
        virtual void updateStep(){;}

        //virtual Watcher* createWatcher() { return nullptr;}}

        virtual QStringList getEnumUids( QString ) { return m_enumUids;}    // For enum properties
        virtual QStringList getEnumNames( QString ) { return m_enumNames; } // For enum properties

        //virtual uint32_t* getStatus() { return m_STATUS; }  // Used my Monitor: All CPUs must use m_STATUS

        virtual void INTERRUPT( uint32_t vector ) { CALL_ADDR( vector ); }
        virtual void CALL_ADDR( uint32_t addr ){;} // Used by MCU Interrupts:: All MCUs should use or override this
        virtual uint32_t RET_ADDR() { return m_RET_ADDR; } // Used by Debugger: All CPUs should use or override this

        virtual uint32_t getPC() { return m_PC; }

        virtual void exitSleep() {;}

    protected:
        eMcu* m_mcu;

        QStringList m_enumUids;  // For enum properties
        QStringList m_enumNames; // For enum properties

        Display* m_display; // Find a place for this

        uint8_t m_retCycles;

        uint32_t m_PC;       // Program Counter  /// All CPUs must use this
        uint8_t* m_STATUS;  // STATUS register  /// All CPUs must use this
        uint32_t m_RET_ADDR; // Last Address in Stack /// All CPUs must use this

        /// Should be in McuCpu:
        uint8_t* m_spl;     // STACK POINTER low byte
        uint8_t* m_sph;     // STACK POINTER high byte
        bool     m_spPre;   // STACK pre-increment?
        int      m_spInc;   // STACK grows up or down? (+1 or -1)
        ///--------------------

        void clear_S_Bit( uint8_t bit) { *m_STATUS &= ~(1<<bit); }
        void set_S_Bit( uint8_t bit )  { *m_STATUS |=   1<<bit; }
        void write_S_Bit( uint8_t bit, bool val )
        {
            if( val ) *m_STATUS |=   1<<bit; \
            else      *m_STATUS &= ~(1<<bit);
        }
};

#endif
