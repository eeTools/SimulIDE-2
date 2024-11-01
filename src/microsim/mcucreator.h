/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUCREATOR_H
#define MCUCREATOR_H

#include <QList>

class Mcu;
class eMcu;
class McuMmu;
class McuRam;
class McuPgm;
class Component;
class QString;
class QDomElement;
class McuPrescaled;
class McuModule;
class Interrupt;
class McuTwi;
class McuSpi;
class ScriptPerif;
class Display;

class McuCreator
{
    public:
        McuCreator();
        ~McuCreator();

        static int createMcu( Mcu* mcuComp, QString name );

    private:
        static int  processFile( QString fileName );
        static void createPgm( QDomElement* e );
        static void createRam( QDomElement* e );
        static void createRom( QDomElement* e );
        //static void createMmu( QDomElement* e );
        static void createCfgWord( QDomElement* e );
        static void createDataBlock( QDomElement* d );
        static void createRegisters( QDomElement* e );
        //static void getRegisters( QDomElement* e, uint16_t offset=0 );
        //static void createProgBlock( QDomElement* p );
        static void createInterrupts( QDomElement* i );
        static void createIntOsc( QDomElement* p );
        static void createMcuPort( QDomElement* p );
        static void createIoPort( QDomElement* p );
        static void createTimer( QDomElement* t );
        static void createOcm( QDomElement* e );
        static void createCcpUnit( QDomElement* c );
        static void createMsspUnit( QDomElement* c );
        static void createUsart( QDomElement* u );
        static void createTwi( QDomElement* e );
        static void createSpi( QDomElement* e );
        static void createUsi( QDomElement* e );
        static void createAdc( QDomElement* e );
        static void createDac( QDomElement* e );
        static void createAcomp( QDomElement* e );
        static void createVref( QDomElement* e );
        static void createWdt( QDomElement* e );
        static void createSleep( QDomElement* e );
        static void createDisplay( QDomElement* e );
        static void createStack( QDomElement* s );
        static void createInterrupt( QDomElement* el );
        static void setInterrupt( QString intName, McuModule* module );
        static void setConfigRegs( QDomElement* u, McuModule* module );
        static void setPrescalers( QString pr, McuPrescaled* module );

        static QString m_CompName;
        static QString m_basePath;
        static QString m_core;

        static bool m_newStack;
        static QDomElement m_stackEl;
        static Mcu* m_mcuComp;
        static eMcu* mcu;

        static McuRam* m_mcuRam;
        static McuPgm* m_mcuPgm;
        static McuTwi* m_twi;
        static McuSpi* m_spi;
        static QList<Display*> m_displays;

        static bool m_console;

        static std::vector<ScriptPerif*> m_scriptPerif;
};

#endif
