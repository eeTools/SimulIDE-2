/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUPGM_H
#define MCUPGM_H

#include "mcumodule.h"
#include "memory.h"
#include "e-element.h"

class McuPgm : public McuModule, public Memory, public eElement
{
        friend class McuCreator;
        friend class Mcu;

    public:
        McuPgm( eMcu* mcu, QString name );
        virtual ~McuPgm();

        virtual void setup() override {;}

        //virtual void initialize() override;
        virtual void reset() override;

        virtual void readPgm();
        virtual void writePgm();

        virtual void addrWriteL( uint8_t val );
        virtual void addrWriteH( uint8_t val );

    protected:

        uint8_t* m_addressL; // Pointer to data for counter Low address byte
        uint8_t* m_addressH; // Pointer to data for counter High address byte

        uint32_t m_address;
        uint8_t* m_dataReg;

        //uint32_t m_size;
        //bool m_savePgm;
};

#endif
