/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QTranslator>
#include <QFileDialog>
#include <QMessageBox>

#include "memory.h"
#include "memtable.h"
#include "simulator.h"
#include "circuitwidget.h"
#include "circuit.h"
/// #include "mcuconfigword.h"
/// #include "e_mcu.h"
#include "utils.h"

Memory::Memory()
{
    m_memTable = nullptr;
    m_eMcu = nullptr;

    m_wordBits  = 8;
    m_wordBytes = 1;
}
Memory::~Memory()
{
    if( !m_memTable ) return;

    /// Fixme: memTable in McuMonitor is deleted there
    ///m_memTable->setParent( nullptr );
    ///m_memTable->deleteLater();
}

uint32_t Memory::getValue( uint32_t addr )
{
    switch( m_wordBytes ) {
        case 1: return read_08( addr );
        case 2: return read_16( addr );
        case 3:
        case 4: return read_32( addr );
    }
    return 0;
}

uint16_t Memory::read_16( uint32_t addr )
{
    //addr = mappedAddr( addr );
    uint16_t data16 = *((uint16_t*) m_rawData+addr);

    return data16;
    //return m_data[addr] | (m_data[addr+1] << 8);
}

uint32_t Memory::read_32( uint32_t addr )
{
    //addr = mappedAddr( addr );
    uint32_t data32 = *((uint32_t*)(m_rawData+addr));
    return data32;

    /*return (  m_data[addr]
           | (m_data[addr+1] << 8)
           | (m_data[addr+2] << 16)
           | (m_data[addr+3] << 24) );*/
}

void Memory::setValue( uint32_t addr, uint32_t val )
{
    switch( m_wordBytes ) {
        case 1: write_08( addr, val ); break;
        case 2: write_16( addr, val ); break;
        case 3:
        case 4: write_32( addr,val );
    }
}

void Memory::write_16( uint32_t addr, uint16_t val )
{
    uint16_t* data = (uint16_t*)m_rawData+addr;
    *data = val;

    //addr = mappedAddr( addr );
    //m_data[addr]   = val & 0xFF;
    //m_data[addr+1] = (val >> 8) & 0xFF;
}

void Memory::write_32( uint32_t addr, uint32_t val )
{
    uint32_t* data32 = (uint32_t*)(m_rawData+addr);
    *data32 = val;
    //addr = mappedAddr( addr );
    //m_data[addr]   = val & 0xFF;
    //m_data[addr+1] = (val >> 8 ) & 0xFF;
    //m_data[addr+2] = (val >> 16) & 0xFF;
    //m_data[addr+3] = (val >> 24) & 0xFF;
}

void Memory::setWordBits( int b )
{
    m_wordBits = b;
    m_wordBytes = (m_wordBits+7)/8;

    ///if( m_memTable ) m_memTable->resizeTable();
}

void Memory::resize( int size )
{
    m_data.resize( size*m_wordBytes );
    m_rawData = m_data.data();
    if( m_memTable ) m_memTable->resizeTable();
}

void Memory::fillMemory( uint32_t v )
{
    /// TODO:
    /// Mask value
    /// Convert to uint8_t

    for( uint i=0; i<m_data.size(); ++i ) m_data[i] = v;
    if( m_memTable ) m_memTable->updateData();
}

void Memory::showTable()
{
    if( !m_memTable )
    {
        m_memTable = new MemTable( CircuitWidget::self(), this, m_wordBytes );
        m_memTable->setWindowFlags( Qt::Window | Qt::WindowTitleHint | Qt::Tool
                                  | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint );
    }
    m_memTable->show();
}

MemTable* Memory::getTable()
{
    showTable();
    return m_memTable;
}

bool Memory::loadData()
{
    Simulator::self()->pauseSim();

    QString dir = changeExt( Circuit::self()->getFilePath(), ".data" );
    QString fileName = QFileDialog::getOpenFileName( nullptr, "Memory::loadData", dir,
                       simulideTr( "Memory", "All files (*.*);;.data (*.data);;.bin (*.bin)"));

    if( fileName.isEmpty() ) return false; // User cancels loading

    bool ok = loadFile( fileName );
    Simulator::self()->resumeSim();

    return ok;
}

bool Memory::loadFile( QString file, eMcu* eMcu )
{
    m_eMcu = eMcu;
    if( eMcu ) return loadHex( file ); // MCUs file must be hex format

    QString ext = getFileExt( file ).toLower();

    bool ok = false;
    if     ( ext == ".data" ) ok = loadDat( file );
    else if( ext == ".hex"
          || ext == ".ihx" )  ok = loadHex( file ); // Intel Hex Format
    else                      ok = loadBin( file ); // Binary Format

    m_eMcu = nullptr;
    return ok;
}

bool Memory::loadDat( QString file )
{
    QString data = fileToString( file, "Memory::loadTxt" );

    data.replace("\n", ",");
    return loadCsv( data );
}

bool Memory::loadCsv( QString data )
{
    int addr = 0;
    int size = m_data.size();

    QStringList words = data.split(",");
    words.removeAll("");

    for( QString sdata : words )
    {
        bool ok = false;
        int data = sdata.toInt( &ok );
        if( !ok ) continue;

        if( addr >= size )
        {
            qDebug() << "\nMemory::loadDat: Data doesn't fit in Memory"<<size<<"\n";
            break;
        }
        m_data.at( addr ) = data ;
        addr++;
    }
    return true;
}

bool Memory::loadHex( QString file )
{
    qDebug() <<"Loading hex file:\n"<<file<<"\n";
    QStringList lineList = fileToStringList( file, "Memory::loadHex" );

    int nLine = 0;
    int addrBase = 0;
    int size = m_data.size();

    bool ok;

    for( QString line : lineList )
    {
        int checksum = 0;
        line = line.remove( " " );
        if( line.isEmpty() ) continue;

        if( !(line.left(1) == ":") ){
            qDebug() << "    Error: Wrong Start code at line "+QString::number(nLine);
            return false;
        }
        line = line.remove( 0, 1 );

        int nBytes = line.left( 2 ).toInt( &ok, 16 );
        int lineSize = 2+4+2+nBytes*2+2;
        if( line.size() != lineSize ){
            qDebug() << "    Error: Wrong line size at line "+QString::number(nLine);
            return false;
        }
        checksum += nBytes;

        int addr = addrBase+line.mid( 2, 4 ).toInt( &ok, 16 );
        //addr /= m_wordBytes;
        checksum += line.mid( 2, 2 ).toInt( &ok, 16 );
        checksum += line.mid( 4, 2 ).toInt( &ok, 16 );

        int type = line.mid( 6, 2 ).toInt( &ok, 16 );
        if     ( type == 1 ) return true; // Reached End Of File
        else if( type == 4 );             // Extended Linear Address
        else if( type != 0 )
        {
            qDebug() <<"    Warning: Not supported Record type:"<<type<<"at line"<<QString::number(nLine);
            continue;
        }
        checksum += type;

        int i;
        for( i=8; i<8+nBytes*2; i+=2/* *m_wordBytes*/ )
        {
            uint8_t data = line.mid( i, 2 ).toInt( &ok, 16 );
            checksum += data;

            /*if( m_wordBytes == 2 ){
                hiByte = line.mid( i+2, 2 ).toInt( &ok, 16 );
                data += (hiByte<<8);
                checksum += hiByte;
            }*/
            if( type == 4 )
            {
                addrBase = (line.mid( 8, 4 ).toInt( &ok, 16 ))<<16;
                qDebug() <<"    Extended Linear Address:"<< "0x"+QString::number( addrBase, 16 ).toUpper();
                continue;
            }

            //qDebug()<< "Memory::loadHex"<<addrBase/bytes<< addr <<data;
            if( addr >= size ){
                bool ok = false;
                /*if( m_eMcu )
                {
                    ConfigWord* cfgWord = (ConfigWord*)m_eMcu->getModule("cfgword");
                    if( cfgWord ) ok = cfgWord->setCfgWord( addr, data );
                }*/
                if( !ok )
                {
                    qDebug() << "    Warning: PGM End reached at Line"<<QString::number( nLine )
                             <<"\n    Address:"<<"0x"+QString::number( addr, 16 )<<"> PMG End:"<<"0x"+QString::number( size-1, 16 ).toUpper()
                             <<"\n    TODO: Config word ??"<<"\n";
                    return true;
                }
            }
            else m_data.at( addr ) = data;
            addr++;
        }
        checksum += line.mid( i, 2 ).toInt( &ok, 16 );
        if( checksum & 0xFF ){
            qDebug() << "    Error: CheckSum Error at line "+QString::number(nLine)+1;
            return false;
        }
        nLine++;
    }
    qDebug() << "    Error: No End Of File reached";
    return false;
}

bool Memory::loadBin( QString file )
{
    int size = m_data.size()-1;

    QByteArray ba = fileToByteArray( file, "Memory::loadData" );
    int memSize = ba.size();

    for( int i=0; i<memSize; i++ )
    {
        if( i >= size ){
            qDebug() << "\nMemory::loadBin: Data doesn't fit in Memory"<<size<<"\n";
            return false;
        }
        m_data.at(i) = (uint8_t)ba.at( i );
    }
    return true;
}

void Memory::saveData()
{
     Simulator::self()->pauseSim();

    QString dir = changeExt( Circuit::self()->getFilePath(), ".data" );

    QString file = QFileDialog::getSaveFileName( nullptr,
                       simulideTr( "Memory", "Save Data" ), dir,
                       simulideTr( "Memory", "All files (*.*);;.data (*.data);;.bin (*.bin)"));

    if( file.isEmpty() ) return; // User cancels saving

    if     ( file.endsWith(".data")
           || file.endsWith(".csv") ) saveCsv( file ); // CSV
    else if( file.endsWith(".hex")  ) saveHex( file );
    else                              saveBin( file ); // Binary Format

    Simulator::self()->resumeSim();
}

void Memory::saveCsv( QString file )
{
    QFile outFile( file );
    if( outFile.open( QFile::WriteOnly | QFile::Text ) )
    {
        QTextStream toFile( &outFile );

        /*for( uint8_t val : m_data )
        {
            toFile << QString::number( val, 16 );
            toFile << ",";
        }*/
        toFile << getCsv();
        outFile.close();
    }else{
        QMessageBox::warning(nullptr, "Memory::saveData",
        simulideTr( "Memory", "Cannot write file %1:\n%2.").arg(file).arg(outFile.errorString()));
    }
}

void Memory::saveHex( QString file ) /// TODO
{

}

void Memory::saveBin( QString file )
{
    QFile outFile( file );
    if( outFile.open( QFile::WriteOnly ) )
    {
        for( char val : m_data ) outFile.write( &val, 1 );
        outFile.close();
    }else{
        QMessageBox::warning(nullptr, "Memory::saveData",
        simulideTr( "Memory", "Cannot write file %1:\n%2.").arg(file).arg(outFile.errorString()));
    }
}

QString Memory::getCsv()
{
    QString m;
    uint size = m_data.size();
    if( size > 0 )
    {
        bool empty = true;
        for( int i=size-1; i>=0; --i )
        {
            uint8_t val = m_data.at( i );
            if( val != 0 ) empty = false;
            if( empty ) continue;
            m.prepend( QString::number( val )+"," );
    }   }
    return m;
}
