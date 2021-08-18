/***************************************************************************
 *   Copyright (C) 2020 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#include <QDomDocument>
#include <QFileInfo>
#include <QObject>

#include "mcucreator.h"
#include "e_mcu.h"
#include "mcu.h"
#include "mcuport.h"
#include "mcupin.h"

#include "usarttx.h"
#include "usartrx.h"

// Cores
#include "avrcore.h"
#include "avrport.h"
#include "avrtimer.h"
#include "avrocunit.h"
#include "avrinterrupt.h"
#include "avrusart.h"
#include "avradc.h"
#include "avrtwi.h"
#include "avrspi.h"
#include "avrwdt.h"
#include "avreeprom.h"
#include "avrcomparator.h"

#include "pic14core.h"
#include "pictimer.h"

#include "i51core.h"
#include "i51timer.h"
#include "i51interrupt.h"
#include "i51usart.h"

#include "utils.h"


QString McuCreator::m_core = "";
QString McuCreator::m_CompName = "";
QString McuCreator::m_basePath = "";
Mcu*    McuCreator::m_mcuComp = NULL;
eMcu*   McuCreator::mcu = NULL;
QDomElement McuCreator::m_stackEl;

McuCreator::McuCreator(){}
McuCreator::~McuCreator(){}

int McuCreator::createMcu( Mcu* mcuComp, QString name )
{
    m_CompName = name;
    m_mcuComp = mcuComp;

    mcu = &(mcuComp->m_eMcu);
    QString dataFile = mcuComp->m_dataFile;
    m_basePath = QFileInfo( dataFile ).absolutePath();
    dataFile = QFileInfo( dataFile ).fileName();

    int error = processFile( dataFile );

    if( error == 0 ) mcu->getRamTable()->setRegisters( mcu->m_regInfo.keys() );
    return error;
}

int McuCreator::processFile( QString fileName )
{
    fileName = m_basePath+"/"+fileName;
    QDomDocument domDoc = fileToDomDoc( fileName, "McuCreator::processFile" );
    if( domDoc.isNull() )
    {
        return 1; }

    QDomElement root = domDoc.documentElement();

    if( root.hasAttribute("core") )       m_core = root.attribute( "core" );
    if( root.hasAttribute("data") )       createDataMem( root.attribute( "data" ).toUInt(0,0) );
    if( root.hasAttribute("prog") )       createProgMem( root.attribute( "prog" ).toUInt(0,0) );
    if( root.hasAttribute("eeprom") )     createRomMem( root.attribute( "eeprom" ).toUInt(0,0) );
    if( root.hasAttribute("progword") )   mcu->m_wordSize = root.attribute( "progword" ).toUInt(0,0);
    if( root.hasAttribute("inst_cycle") ) mcu->m_cPerInst = root.attribute( "inst_cycle" ).toDouble();

    int error = 0;
    QDomNode node = root.firstChild();
    while( !node.isNull() )
    {
        QDomElement el = node.toElement();

        if     ( el.tagName() == "regblock" )   createRegisters( &el );
        else if( el.tagName() == "datablock" )  createDataBlock( &el );
        else if( el.tagName() == "stack" )      m_stackEl = el;
        //else if( el.tagName() == "status" )     createStatusReg( &el );
        else if( el.tagName() == "interrupts" ) createInterrupts( &el );
        else if( el.tagName() == "port" )       createPort( &el );
        else if( el.tagName() == "timer" )      createTimer( &el );
        else if( el.tagName() == "usart" )      createUsart( &el );
        else if( el.tagName() == "adc" )        createAdc( &el );
        else if( el.tagName() == "acomp" )      createAcomp( &el );
        else if( el.tagName() == "twi" )        createTwi( &el );
        else if( el.tagName() == "spi" )        createSpi( &el );
        else if( el.tagName() == "wdt" )        createWdt( &el );
        else if( el.tagName() == "eeprom" )     createEeprom( &el );
        else if( el.tagName() == "include" )
        {
            error = processFile( el.attribute("file") );
            if( error ) return error;
        }

        node = node.nextSibling();
    }
    if( root.hasAttribute("core") ) createCore( m_core );

    return 0;
}

void McuCreator::createProgMem( uint32_t size )
{
    mcu->m_flashSize = size;
    mcu->m_progMem.resize( size, 0xFFFF );
}

void McuCreator::createDataMem( uint32_t size )
{
    mcu->m_ramSize = size;
    mcu->m_dataMem.resize( size, 0 );
    mcu->m_addrMap.resize( size, 0xFFFF ); // Not Maped values = 0xFFFF -> don't exist
}

void McuCreator::createRomMem( uint32_t size )
{
    if( size == 0 ) return;

    mcu->m_romSize = size;
    mcu->m_eeprom.resize( size );
    mcu->m_eeprom.fill( 0xFF );

}

void McuCreator::createEeprom(  QDomElement* e )
{
    McuEeprom* eeprom = NULL;
    QString eepromName = e->attribute("name");

    if( m_core == "AVR" )  eeprom = new AvrEeprom( mcu, eepromName );
    else return;

    mcu->m_modules.emplace_back( eeprom );

    setConfigRegs( e, eeprom );

    if( e->hasAttribute("dataregs") )
    {
        QString datareg = e->attribute("dataregs");
        eeprom->m_dataReg = mcu->getReg( datareg );
    }

    if( e->hasAttribute("addressreg") )
    {
        QString addrReg = e->attribute("addressreg");
        QString lowByte = addrReg;
        QString highByte ="";

        if( addrReg.contains(",") )
        {
            QStringList regs = addrReg.split(",");

            lowByte  = regs.takeFirst();
            highByte = regs.takeFirst();
        }
        if( !lowByte.isEmpty() )
        {
            eeprom->m_addressL = mcu->getReg( lowByte );
            mcu->watchRegNames( lowByte, R_WRITE, eeprom, &McuEeprom::addrWriteL );
        }
        if( !highByte.isEmpty() )
        {
            eeprom->m_addressH = mcu->getReg( highByte );
            mcu->watchRegNames( highByte, R_WRITE, eeprom, &McuEeprom::addrWriteH );
    }   }
    QDomNode node = e->firstChild();
    while( !node.isNull() )
    {
        QDomElement el = node.toElement();

        if( el.tagName() == "interrupt" ) setInterrupt( &el, eeprom );
        node = node.nextSibling();
}   }

void McuCreator::createDataBlock( QDomElement* d )
{
    uint16_t datStart = d->attribute("start").toUInt(0,0);
    uint16_t datEnd   = d->attribute("end").toUInt(0,0);
    uint16_t mapTo    = datStart;

    if( datEnd >= mcu->m_ramSize )
    {
        qDebug() << "McuCreator::createDataBlock  ERROR creating DataBlock";
        qDebug() << "dataMemSize  = " << mcu->m_ramSize;
        qDebug() << "dataBlockEnd = " << datEnd;
        return;
    }
    if( d->hasAttribute("mapto") ) mapTo = d->attribute("mapto").toUInt(0,0);

    for( int i=datStart; i<=datEnd; ++i )
    {
        mcu->m_addrMap[i] = mapTo;
        mapTo++;
}   }

void McuCreator::createRegisters( QDomElement* e )
{
    uint16_t regStart = e->attribute("start").toUInt(0,0);
    uint16_t regEnd   = e->attribute("end").toUInt(0,0);
    uint16_t offset   = e->attribute("offset").toUInt(0,0);
    QString stReg;
    if( e->hasAttribute( "streg" ) ) stReg = e->attribute( "streg" );

    if( regEnd >= mcu->m_ramSize )
    {
        qDebug() << "McuCreator::createRegisters  ERROR creating Registers";
        qDebug() << "dataMemSize  = " << mcu->m_ramSize;
        qDebug() << "RegistersEnd = " << regEnd;
        return;
    }

    if( regStart < mcu->m_regStart ) mcu->m_regStart = regStart;
    if( regEnd   > mcu->m_regEnd )
    {
        mcu->m_regEnd = regEnd;
        mcu->m_regMask.resize( regEnd, 0xFF );
    }

    QDomNode node = e->firstChild();
    while( !node.isNull() ) // Create Registers
    {
        QDomElement el = node.toElement();

        if( el.tagName() == "register" )
        {
            QString  regName = el.attribute("name");
            uint16_t regAddr = el.attribute("addr").toUInt(0,0)+offset;
            uint8_t resetVal = el.attribute("reset").toUInt(0,0);

            if( el.hasAttribute("mask") )
            {
                uint8_t writeMask = el.attribute("mask").toUInt(0,0);
                mcu->m_regMask[regAddr] = writeMask;
            }

            mcu->m_addrMap[regAddr] = regAddr;

            eMcu::regInfo_t regInfo = { regAddr, resetVal/*, writeMask*/ };
            mcu->m_regInfo.insert( regName, regInfo );

            QString bits = el.attribute( "bits" );
            if( !bits.isEmpty() )                    // Create bitMasks
            {
                QString bitName;
                if( bits == "0-7"){
                    for( int i=0; i<8; ++i )
                    {
                        bitName = regName+QString::number( i );
                        mcu->m_bitMasks.insert( bitName, 1<<i );
                        mcu->m_bitRegs.insert( bitName, regAddr );
                    }
                }else{
                    QStringList bitList = bits.split(",");
                    for( int i=0; i<bitList.size(); ++i )
                    {
                        bitName = bitList.at( i );
                        if( bitName == "0" ) continue;
                        mcu->m_bitMasks.insert( bitName, 1<<i );
                        mcu->m_bitRegs.insert( bitName, regAddr );
                    }
                    if( !stReg.isEmpty() && ( regName == stReg ) )
                    {
                        mcu->m_sregAddr = regAddr;
                        mcu->getRamTable()->setStatusBits( bitList );
            }   }   }
        }else if( el.tagName() == "alias" )
        {
            uint16_t regAddr = el.attribute("addr").toUInt(0,0)+offset;
            uint16_t mapTo   = el.attribute("mapto").toUInt(0,0);
            mcu->m_addrMap[regAddr] = mapTo;
        }
        node = node.nextSibling();
}   }

void McuCreator::createInterrupts( QDomElement* i )
{
    QString enable = i->attribute("enable");
    mcu->watchBitNames( enable, R_WRITE, (eMcu*)mcu, &eMcu::enableInterrupts );

    QDomNode node = i->firstChild();
    while( !node.isNull() )
    {
        QDomElement el = node.toElement();
        if( el.tagName() == "interrupt" ) createInterrupt( &el );
        node = node.nextSibling();
}   }

void McuCreator::createPort( QDomElement* p )
{
    QString name = p->attribute( "name" );
    uint8_t numPins = p->attribute( "pins" ).toUInt(0,0);

    McuPort* port;;
    if( m_core == "AVR" ) port = new AvrPort( mcu, name, numPins );
    else                  port = new McuPort( mcu, name, numPins );
    mcu->m_ports.m_portList.insert( name, port );
    mcu->m_modules.emplace_back( port );
    port->createPins( m_mcuComp );

    uint16_t addr = 0;
    if( p->hasAttribute( "outreg" ) ) // Connect to PORT Out Register
    {
        addr = mcu->getRegAddress( p->attribute( "outreg" ) );
        port->m_outAddr = addr;
        port->m_outReg  = mcu->m_dataMem.data()+addr;
        mcu->watchRegister( addr, R_WRITE, port, &McuPort::outChanged );
    }
    if( p->hasAttribute( "inreg" ) ) // Connect to PORT In Register
    {
        addr = mcu->getRegAddress( p->attribute( "inreg" ) );
        port->m_inAddr = addr;
    }
    if( p->hasAttribute( "dirreg" ) ) // Connect to PORT Dir Register
    {
        QString dirreg = p->attribute( "dirreg" );
        if( dirreg.startsWith("!") )
        {
            port->m_dirInv = true;
            dirreg.remove( 0, 1 );
        }
        addr = mcu->getRegAddress( dirreg );
        port->m_dirAddr = addr;
        port->m_dirReg  = mcu->m_dataMem.data()+addr;
        mcu->watchRegister( addr, R_WRITE, port, &McuPort::dirChanged );
    }
    if( p->hasAttribute("outmask") ) // Permanent Outputs
    {
        uint8_t outMask = p->attribute("outmask").toUInt( 0, 2 );
        for( int i=0; i<port->m_numPins; ++i )
            port->m_pins[i]->m_outMask = outMask & 1<<i;
    }
    if( p->hasAttribute("inpmask") ) // Permanent Inputs
    {
        uint8_t inpMask = p->attribute("inpmask").toUInt( 0, 2 );
        for( int i=0; i<port->m_numPins; ++i )
            port->m_pins[i]->m_inpMask = inpMask & 1<<i;
    }
    if( p->hasAttribute("pullups") ) // Permanent Pullups
    {
        uint8_t pullup = p->attribute("pullups").toUInt( 0, 2 );
        for( int i=0; i<port->m_numPins; ++i )
            port->m_pins[i]->m_puMask = pullup & 1<<i;
    }
    if( p->hasAttribute("opencol") ) // OPen Drain
    {
        uint8_t opencol = p->attribute("opencol").toUInt( 0, 2 );
        for( int i=0; i<port->m_numPins; ++i )
            if( opencol & 1<<i ) port->m_pins[i]->m_openColl = true;
    }
    // Interrupts and...
    QDomNode node = p->firstChild();
    while( !node.isNull() )
    {
        QDomElement el = node.toElement();

        if( el.tagName() == "interrupt" )
        {
            setInterrupt( &el, port ); // Pin change interrupt
            if( el.hasAttribute("mask") )
            {
                uint16_t maskReg = mcu->getRegAddress( el.attribute( "mask" ) );
                mcu->watchRegister( maskReg, R_WRITE, port, &McuPort::intMaskChanged );
        }   }
        node = node.nextSibling();
}   }

void McuCreator::createTimer( QDomElement* t )
{
    McuTimer* timer = NULL;
    QString timerName = t->attribute("name");

    if     ( m_core == "8051" ) timer = new I51Timer( mcu, timerName );
    else if( m_core == "AVR" )  timer = AvrTimer::makeTimer( mcu, timerName );
    else if( m_core == "Pic14" ) timer = PicTimer::makeTimer( mcu, timerName );
    else return;

    mcu->m_timers.m_timerList.insert( timerName, timer );
    mcu->m_modules.emplace_back( timer );

    if( t->hasAttribute("counter") )
    {
        QString counter = t->attribute("counter");
        QString lowByte = counter;
        QString highByte ="";

        if( counter.contains(",") )
        {
            QStringList regs = counter.split(",");

            lowByte = regs.takeFirst();
            highByte = regs.takeFirst();
        }
        if( !lowByte.isEmpty() )
        {
            timer->m_countL = mcu->getReg( lowByte );
            mcu->watchRegNames( lowByte, R_WRITE, timer, &McuTimer::countWriteL );
            mcu->watchRegNames( lowByte, R_READ,  timer, &McuTimer::updtCount );
        }
        if( !highByte.isEmpty() )
        {
            timer->m_countH = mcu->getReg( highByte );
            mcu->watchRegNames( highByte, R_WRITE, timer, &McuTimer::countWriteH );
            mcu->watchRegNames( highByte, R_READ,  timer, &McuTimer::updtCount );
        }
    }
    if( t->hasAttribute("enable") )
    {
        QString enable = t->attribute("enable");
        mcu->watchBitNames( enable, R_WRITE, timer, &McuTimer::enable );
    }
    setConfigRegs( t, timer );

    QDomNode node = t->firstChild();
    while( !node.isNull() )
    {
        QDomElement el = node.toElement();

        if     ( el.tagName() == "interrupt" )  setInterrupt( &el, timer );
        else if( el.tagName() == "prescaler" )
        {
            QStringList prescalers = el.attribute("values").remove(" ").split(",");
            timer->m_prescList.resize( prescalers.size() );

            for( int i=0; i<prescalers.size(); ++i )
                timer->m_prescList[i] = prescalers.at(i).toUInt();
        }
        else if( el.tagName() == "extclock" )
        {
            /// TODO
        }
        else if( el.tagName() == "ocunit" )
        {
            McuOcUnit* ocUnit = NULL;
            if( m_core == "AVR" ) ocUnit = new AvrOcUnit( mcu, el.attribute("name") );
            if( !ocUnit ) continue;

            McuPin* pin = mcu->m_ports.getPin( el.attribute("pin") );

            timer->addOcUnit( ocUnit );
            ocUnit->m_timer = timer;
            ocUnit->m_ocPin = pin;

            QString ocrReg = el.attribute("ocreg");
            //mcu->watchRegNames( ocrRegName, R_WRITE, ocUnit, &McuOcUnit::ocrChanged );

            QString lowByte = ocrReg;
            QString highByte ="";

            if( ocrReg.contains(",") )
            {
                QStringList regs = ocrReg.split(",");

                lowByte = regs.takeFirst();
                highByte = regs.takeFirst();
            }
            if( !lowByte.isEmpty() )
                mcu->watchRegNames( lowByte, R_WRITE, ocUnit, &McuOcUnit::ocrWriteL );

            if( !highByte.isEmpty() )
                mcu->watchRegNames( highByte, R_WRITE, ocUnit, &McuOcUnit::ocrWriteH );

            if( el.hasAttribute("configbits") )
            {
                QString configBits = el.attribute("configbits");
                //mcu->watchBitNames( configBits, R_WRITE, ocUnit, &McuOcUnit::configure );
                ocUnit->m_configBits = mcu->getRegBits( configBits );
            }
            QDomNode node1 = el.firstChild();
            while( !node1.isNull() )
            {
                QDomElement el1 = node1.toElement();
                if( el1.tagName() == "interrupt" )  setInterrupt( &el1, ocUnit );

                node1 = node1.nextSibling();
        }   }
        node = node.nextSibling();
}   }

void McuCreator::createUsart( QDomElement* u )
{
    QString name = u->attribute( "name" );
    int   number = u->attribute( "number" ).toInt();

    McuUsart* usartM;
    if     ( m_core == "8051" ) usartM = new I51Usart( mcu, name, number );
    else if( m_core == "AVR" )  usartM = new AvrUsart( mcu, name, number );
    else return;

    mcu->m_usarts.emplace_back( usartM ); //  .m_usartList.insert( name, usartM );

    setConfigRegs( u, usartM );

    QDomNode node = u->firstChild();
    while( !node.isNull() )
    {
        QDomElement el = node.toElement();

        if( el.tagName() == "trunit" )
        {
            UartTR* trUnit;
            QString type = el.attribute( "type" );
            if     ( type == "tx" ) trUnit = usartM->m_sender;
            else if( type == "rx" ) trUnit = usartM->m_receiver;
            else continue;

            mcu->m_modules.emplace_back( trUnit );

            if( type == "tx" )
            {
                QString regName = el.attribute( "register" );
                mcu->watchRegNames( regName, R_WRITE, usartM, &McuUsart::sendByte );
                mcu->watchRegNames( regName, R_READ,  usartM, &McuUsart::readByte );
            }

            QString pinName = el.attribute( "pin" );
            trUnit->setPin( mcu->m_ports.getPin( pinName ) );

            if( el.hasAttribute("enable") )
            {
                QString enable = el.attribute( "enable" );
                mcu->watchBitNames( enable, R_WRITE, trUnit, &UartTR::enable );
            }
            QDomNode node1 = el.firstChild();
            while( !node1.isNull() )
            {
                QDomElement el1 = node1.toElement();
                if( el1.tagName() == "interrupt" ) setInterrupt( &el1, trUnit );

                node1 = node1.nextSibling();
        }   }
        else if( el.tagName() == "interrupt" ) setInterrupt( &el, usartM );
        node = node.nextSibling();
}   }

void McuCreator::createAdc( QDomElement* e )
{
    QString name = e->attribute( "name" );
    McuAdc* adc;
    if( m_core == "AVR" ) adc = new AvrAdc( mcu, name );
    else return;

    mcu->m_modules.emplace_back( adc );

    setConfigRegs( e, adc );

    if( e->hasAttribute("bits") )
    {
        bool ok = false;
        int bits = e->attribute("bits").toInt( &ok );
        if( ok ) adc->m_maxValue = pow( 2, bits )-1;
    }
    if( e->hasAttribute("dataregs") )
    {
        QString dataregs = e->attribute("dataregs");
        QString lowByte = dataregs;
        QString highByte ="";

        if( dataregs.contains(",") )
        {
            QStringList regs = dataregs.split(",");

            lowByte = regs.takeFirst();
            highByte = regs.takeFirst();
        }
        if( !lowByte.isEmpty() )  adc->m_ADCL = mcu->getReg( lowByte );
        if( !highByte.isEmpty() ) adc->m_ADCH = mcu->getReg( highByte );
    }
    if( e->hasAttribute("multiplex") )
    {
        QString configRegs = e->attribute("multiplex");
        mcu->watchRegNames( configRegs, R_WRITE, adc, &McuAdc::setChannel );
    }

    QDomNode node = e->firstChild();
    while( !node.isNull() )
    {
        QDomElement el = node.toElement();

        if     ( el.tagName() == "interrupt" ) setInterrupt( &el, adc );
        else if( el.tagName() == "prescaler" )
        {
            QStringList prescalers = el.attribute("values").remove(" ").split(",");
            adc->m_prescList.resize( prescalers.size() );

            for( int i=0; i<prescalers.size(); ++i )
                adc->m_prescList[i] = prescalers.at(i).toUInt();
        }
        else if( el.tagName() == "inputs" )
        {
            QString type = el.attribute("type");
            if( type == "PIN" )
            {
                QStringList pins = el.attribute("source").remove(" ").split(",");
                for( QString pinName : pins )
                {
                    McuPin* pin = mcu->m_ports.getPin( pinName );
                    if( pin ) adc->m_adcPin.emplace_back( pin );
        }   }   }
        node = node.nextSibling();
}   }

void McuCreator::createAcomp( QDomElement* e )
{
    QString name = e->attribute( "name" );
    McuComp* comp;
    if( m_core == "AVR" ) comp = new AvrComp( mcu, name );
    else return;

    mcu->m_modules.emplace_back( comp );

    setConfigRegs( e, comp );

    QDomNode node = e->firstChild();
    while( !node.isNull() )
    {
        QDomElement el = node.toElement();

        if     ( el.tagName() == "interrupt" ) setInterrupt( &el, comp );
        else if( el.tagName() == "inputpin" )
        {
            QString pinName = el.attribute("pin");
            McuPin* pin = mcu->m_ports.getPin( pinName );
            if( pin )
            {
                QString name = el.attribute("name");
                if     ( name == "positive" ) comp->m_pinP = pin ;
                else if( name == "negative" ) comp->m_pinN = pin ;
        }   }
        node = node.nextSibling();
}   }

void McuCreator::createTwi( QDomElement* e )
{
    QString name = e->attribute( "name" );
    McuTwi* twi;
    if( m_core == "AVR" ) twi = new AvrTwi( mcu, name );
    else return;

    mcu->m_modules.emplace_back( twi );

    setConfigRegs( e, twi );
    if( e->hasAttribute("dataregs") )
    {
        QString dataReg = e->attribute("dataregs");
        if( !dataReg.isEmpty() )  twi->m_dataReg = mcu->getReg( dataReg );
        mcu->watchRegNames( dataReg, R_WRITE, twi, &McuTwi::writeTwiReg );
    }
    if( e->hasAttribute("addressreg") )
    {
        QString addrReg = e->attribute("addressreg");
        if( !addrReg.isEmpty() )
        {
            twi->m_addrReg = mcu->getReg( addrReg );
            mcu->watchRegNames( addrReg, R_WRITE, twi, &McuTwi::writeAddrReg );
    }   }
    if( e->hasAttribute("statusreg") )
    {
        QString statReg = e->attribute("statusreg");
        if( !statReg.isEmpty() )
        {
            twi->m_statReg = mcu->getReg( statReg );
            mcu->watchRegNames( statReg, R_WRITE, twi, &McuTwi::writeStatus );
    }   }
    QDomNode node = e->firstChild();
    while( !node.isNull() )
    {
        QDomElement el = node.toElement();

        if     ( el.tagName() == "interrupt" ) setInterrupt( &el, twi );
        else if( el.tagName() == "prescaler" )
        {
            QStringList prescalers = el.attribute("values").remove(" ").split(",");
            twi->m_prescList.resize( prescalers.size() );

            for( int i=0; i<prescalers.size(); ++i )
                twi->m_prescList[i] = prescalers.at(i).toUInt();
        }
        else if( el.tagName() == "inputpin" )
        {
            QString pinName = el.attribute("pin");
            McuPin* pin = mcu->m_ports.getPin( pinName );
            if( pin )
            {
                QString name = el.attribute("name");
                if     ( name == "sda" ) twi->setSdaPin( pin );
                else if( name == "scl" ) twi->setSclPin( pin );
        }   }
        node = node.nextSibling();
}   }

void McuCreator::createSpi( QDomElement* e )
{
    QString name = e->attribute( "name" );
    McuSpi* spi;
    if( m_core == "AVR" ) spi = new AvrSpi( mcu, name );
    else return;

    mcu->m_modules.emplace_back( spi );

    setConfigRegs( e, spi );
    if( e->hasAttribute("dataregs") )
    {
        QString dataReg = e->attribute("dataregs");
        if( !dataReg.isEmpty() )  spi->m_dataReg = mcu->getReg( dataReg );
        mcu->watchRegNames( dataReg, R_WRITE, spi, &McuSpi::writeSpiReg );
    }
    if( e->hasAttribute("statusreg") )
    {
        QString statReg = e->attribute("statusreg");
        if( !statReg.isEmpty() )
        {
            spi->m_statReg = mcu->getReg( statReg );
            mcu->watchRegNames( statReg, R_WRITE, spi, &McuSpi::writeStatus );
    }   }
    QDomNode node = e->firstChild();
    while( !node.isNull() )
    {
        QDomElement el = node.toElement();

        if     ( el.tagName() == "interrupt" ) setInterrupt( &el, spi );
        else if( el.tagName() == "prescaler" )
        {
            QStringList prescalers = el.attribute("values").remove(" ").split(",");
            spi->m_prescList.resize( prescalers.size() );

            for( int i=0; i<prescalers.size(); ++i )
                spi->m_prescList[i] = prescalers.at(i).toUInt();
        }
        else if( el.tagName() == "inputpin" )
        {
            QString pinName = el.attribute("pin");
            McuPin* pin = mcu->m_ports.getPin( pinName );
            if( pin )
            {
                QString name = el.attribute("name");
                if     ( name == "mosi" ) spi->setMosiPin( pin );
                else if( name == "miso" ) spi->setMisoPin( pin );
                if     ( name == "sck" )  spi->setSckPin( pin );
                else if( name == "ss" )   spi->setSsPin( pin );
        }   }
        node = node.nextSibling();
}   }

void McuCreator::createWdt( QDomElement* e )
{
    QString name = e->attribute( "name" );
    McuWdt* wdt;
    if( m_core == "AVR" ) wdt = new AvrWdt( mcu, name );
    else return;

    mcu->m_modules.emplace_back( wdt );

    setConfigRegs( e, wdt );
    QDomNode node = e->firstChild();
    while( !node.isNull() )
    {
        QDomElement el = node.toElement();

        if     ( el.tagName() == "interrupt" ) setInterrupt( &el, wdt );
        else if( el.tagName() == "prescaler" )
        {
            QStringList prescalers = el.attribute("values").remove(" ").split(",");
            wdt->m_prescList.resize( prescalers.size() );

            for( int i=0; i<prescalers.size(); ++i )
                wdt->m_prescList[i] = prescalers.at(i).toUInt();
        }
        node = node.nextSibling();
}   }

void McuCreator::createCore( QString core )
{
    if     ( core == "AVR" )   mcu->cpu = new AvrCore( mcu );
    else if( core == "Pic14" ) mcu->cpu = new Pic14Core( mcu );
    else if( core == "8051" )  mcu->cpu = new I51Core( mcu );

    if( !m_stackEl.isNull() ) createStack( &m_stackEl );
}

void McuCreator::createStack( QDomElement* s )
{
    QString spReg = s->attribute("spreg");
    QStringList spRegs = spReg.split(",");

    spReg = spRegs.takeFirst();
    mcu->cpu->m_spl = mcu->getReg( spReg );
    if( !spRegs.isEmpty() )
    {
        spReg = spRegs.takeFirst();
        mcu->cpu->m_sph = mcu->getReg( spReg );
    }
    QString inc = s->attribute("increment");

    mcu->cpu->m_spPre = inc.contains("pre");
    mcu->cpu->m_spInc = ((inc.contains("inc"))?  1:-1);
}

void McuCreator::createInterrupt( QDomElement* el )
{
    QString  intName = el->attribute("name");
    if( intName.isEmpty() ) return;

    uint16_t intVector  = el->attribute("vector").toUInt(0,0);

    Interrupt* iv = NULL;
    if     ( m_core == "8051" ) iv = I51Interrupt::getInterrupt( intName, intVector, (eMcu*)mcu );
    else if( m_core == "AVR" )  iv = AVRInterrupt::getInterrupt( intName, intVector, (eMcu*)mcu );
    else if( m_core == "Pic14" ) iv = new Interrupt( intName, intVector, (eMcu*)mcu );
    if( !iv ) return;

    mcu->m_interrupts.m_intList.insert( intName, iv );
    iv->m_interrupts = &(mcu->m_interrupts);

    QString enable = el->attribute("enable");
    mcu->watchBitNames( enable, R_WRITE, iv, &Interrupt::enableFlag );

    QString intFlag = el->attribute("flag");
    iv->m_flagMask = mcu->m_bitMasks.value( intFlag );
    iv->m_flagReg  = mcu->m_bitRegs.value( intFlag );

    QString intPrio = el->attribute("priority");
    bool ok = false;
    uint8_t prio = intPrio.toUInt(&ok,0);
    if( ok ) iv->setPriority( prio );
    else     mcu->watchBitNames( intPrio, R_WRITE, iv, &Interrupt::setPriority );

    if( el->hasAttribute("mode") )
    {
        QString mode = el->attribute("mode");
        mcu->watchBitNames( mode, R_WRITE, iv, &Interrupt::setMode );
}   }

void McuCreator::setInterrupt( QDomElement* el, McuModule* module )
{
    QString intName = el->attribute("name");
    Interrupt* inte = mcu->m_interrupts.m_intList.value( intName );
    module->m_interrupt = inte;
}

void McuCreator::setConfigRegs( QDomElement* u, McuModule* module )
{
    if( u->hasAttribute("configregsA") )
    {
        QString regs = u->attribute("configregsA");
        mcu->watchRegNames( regs, R_WRITE, module, &McuModule::configureA );
    }
    if( u->hasAttribute("configregsB") )
    {
        QString regs = u->attribute("configregsB");
        mcu->watchRegNames( regs, R_WRITE, module, &McuModule::configureB );
    }
    if( u->hasAttribute("configbitsA") )
    {
        QString configBits = u->attribute("configbitsA");
        mcu->watchBitNames( configBits, R_WRITE, module, &McuModule::configureA );
        module->m_configBitsA = mcu->getRegBits( configBits );
    }
    if( u->hasAttribute("configbitsB") )
    {
        QString configBits = u->attribute("configbitsB");
        mcu->watchBitNames( configBits, R_WRITE, module, &McuModule::configureB );
        module->m_configBitsB = mcu->getRegBits( configBits );
}   }
