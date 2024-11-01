/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCURAM_H
#define MCURAM_H

#include <QMap>
#include <vector>
#include <QDebug>

#include "mcummu.h"
//#include "mcutypes.h"

enum{
    R_READ = 0,
    R_WRITE
};

class Watchable;

class McuRam : public McuMmu
{
    friend class McuCreator;

    public:
        McuRam( eMcu* mcu, QString name );
        ~McuRam();

        virtual void setup() override {;}
        void initialize();

        void addRegBlock( McuRegBlock* b, uint32_t addr );

        McuRegister* getRegByName( QString regName );
        McuRegister* getRegByAddr( uint32_t addr );
        McuRegister* getRegByBits( QString bits );

        regBits_t getRegBits( QString bits );
        uint32_t  getBitMask( QString bits );

        template <class T>                // Add callback for Register changes by reg pointer
        void watchRegister( McuRegister* reg, int write, T* obj, void (T::*func)() )
        {
            if( write ) reg->addWriteCallback( obj, func );
            else        reg->addReadCallback( obj, func );
        }
        template <class T>                // Add callback for Register changes by reg address
        void watchRegAddr( uint16_t addr, int write, T* obj, void (T::*func)() )
        {
            if( addr == 0 ) qDebug() << "Warning: watchRegister address 0 ";

            McuRegister* reg = getRegByAddr( addr );
            if( reg ) watchRegister( reg, write, obj, func );
            else      qDebug() << "Error: watchRegister, No Register at Address" << addr;
        }
        template <class T>                // Add callback for Register changes by reg name
        void watchRegName( QString regName, int write, T* obj, void (T::*func)() )
        {
            if( regName.isEmpty() ) return;

            McuRegister* reg = getRegByName( regName );

            if( reg ) watchRegister( reg, write, obj, func );
            else      qDebug() << "ERROR: Register not found: " << reg;
        }
        template <class T>              // Add callback for Register bit changes by bit names
        void watchBitNames( QString bitNames, int write, T* obj, void (T::*func)() )
        {
            if( bitNames.isEmpty() ) return;
            McuRegister* reg = getRegByBits( bitNames );
            if( reg ) watchRegister( reg, write, obj, func );
        }

        //uint32_t getMapperAddr( uint32_t addr ) { return m_addrMap[addr]; } // Get mapped addresses in Data space
        //uint32_t getRegAddress( QString reg );                              // Get Reg address by name
        uint8_t*  getReg( QString reg );                                   // Get pointer to Reg data by name

        //QStringList registerList() { return m_regInfo.keys(); }

        uint32_t sregAddr() { return m_sregAddr; }

        void setStatusBits( QStringList bits ) { m_statusBits = bits; }
        QStringList getStatusBits() { return m_statusBits; }

        bool isCpuRead() { return m_isCpuRead; }

    protected:

        bool m_isCpuRead;

        //std::vector<uint32_t> m_addrMap;          // Maps addresses in Data space

        QMap<QString, uint32_t> m_bitMasks;        // Access Bit mask by bit name
        QMap<QString, McuRegister*> m_bitRegs;     // Access Register by bit name

        uint32_t m_sregAddr;                       // STATUS Reg Address
        QStringList m_statusBits;


        std::vector<McuRegBlock*> m_regBlocks;
};

#endif
