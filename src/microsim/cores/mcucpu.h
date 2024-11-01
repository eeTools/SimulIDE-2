/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUCPU_H
#define MCUCPU_H

#include "cpubase.h"
#include "mcuram.h"

class McuPgm;

class McuCpu : public CpuBase
{
    public:
        McuCpu( eMcu* mcu );
        ~McuCpu();

        virtual Watcher* getWatcher() override;

        virtual void CALL_ADDR( uint32_t addr ) override; // Used by MCU Interrupts:: All MCUs should use or override this

    protected:
        McuRam* m_mcuRam;
        McuPgm* m_mcuPgm;

        uint32_t  m_progAddrBytes;

        //uint8_t* m_dataMem;
        //uint32_t  m_dataMemEnd;

        //uint32_t  m_regStart;
        //uint32_t  m_regEnd;

        //uint8_t* m_progMem;
        uint32_t  m_progSize;


        //uint32_t  m_lowDataMemEnd;

        /*uint8_t* m_spl;     // STACK POINTER low byte
        uint8_t* m_sph;     // STACK POINTER high byte
        bool     m_spPre;   // STACK pre-increment?
        int      m_spInc;   // STACK grows up or down? (+1 or -1)*/

        virtual void setPC( uint32_t pc ) { m_PC = pc; }

        void RETI()
        {
            m_mcu->m_interrupts.retI();
            RET();
        }
        void RET()
        {
            setPC( POP_STACK() );
            m_mcu->cyclesDone = m_retCycles;
        }

        virtual uint8_t GET_RAM( uint32_t addr )                   // All MCUs should use this
        {
            return m_mcuRam->read_08( addr );
        }
        virtual void SET_RAM( uint32_t addr, uint8_t v )           // All MCUs should use this
        {
            m_mcuRam->write_08( addr, v );
        }

        void SET_REG16_LH( uint32_t addr, uint16_t val )
        {
            m_mcuRam->write_08( addr, val );
            m_mcuRam->write_08( addr+1, val>>8 );
        }
        void SET_REG16_HL( uint32_t addr, uint16_t val )
        {
            m_mcuRam->write_08( addr+1, val>>8 );
            m_mcuRam->write_08( addr , val );
        }
        uint32_t GET_REG16_LH( uint32_t addr )
        {
            return m_mcuRam->read_16( addr );
        }

        // STACK-------------------------------------------------

        uint32_t GET_SP()                         // All MCUs should use this (used by MCU Interrupts).
        {
            uint32_t sp = REG_SPL;
            if( m_sph ) sp |= (REG_SPH << 8);
            return  sp;
        }

        void SET_SP( uint32_t sp )                // All MCUs should use this (used by MCU Interrupts).
        {
            REG_SPL = sp & 0xFF;
            if( m_sph ) REG_SPH = (sp>>8) & 0xFF;
        }

        virtual void PUSH_STACK( uint32_t addr )  // All MCUs should use this (used by MCU Interrupts).
        {
            uint32_t sp = GET_SP();
            if( m_spPre ) sp += m_spInc;

            for( uint i=0; i<m_progAddrBytes; i++, addr>>=8, sp += m_spInc )
                SET_RAM( sp, addr & 0xFF );

            if( m_spPre ) sp -= m_spInc;
            SET_SP( sp );
        }

        virtual uint32_t POP_STACK()
        {
            uint32_t sp = GET_SP();
            uint32_t res = 0;

            if( !m_spPre ) sp -= m_spInc;

            for( uint i=0; i<m_progAddrBytes; i++, sp -= m_spInc )
                res = (res<<8) | GET_RAM( sp );

            if( !m_spPre ) sp += m_spInc;
            SET_SP( sp );
            return res;
        }

        void PUSH_STACK8( uint8_t v )
        {
            uint32_t sp = GET_SP();

            if( m_spPre ) sp += m_spInc;
            SET_RAM( sp, v );
            if( !m_spPre ) sp += m_spInc;

            SET_SP( sp );
        }

        uint8_t POP_STACK8()
        {
            uint32_t sp = GET_SP();

            if( !m_spPre ) sp -= m_spInc;
            uint8_t res = GET_RAM( sp );
            if( m_spPre ) sp -= m_spInc;

            SET_SP( sp );
            return res;
        }
};
#endif
