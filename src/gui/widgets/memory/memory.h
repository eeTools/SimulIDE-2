/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <QString>

class MemTable;
class eMcu;

class Memory
{
    friend class MemTable;

    public:
        Memory();
        ~Memory();

        uint32_t getValue( uint32_t addr );
        virtual uint8_t  read_08( uint32_t addr ) { return m_data[addr]; }
        virtual uint16_t read_16( uint32_t addr );
        virtual uint32_t read_32( uint32_t addr );

        void setValue( uint32_t addr, uint32_t val );
        virtual void write_08( uint32_t addr, uint8_t val ) { m_data[addr] = val; }
        virtual void write_16( uint32_t addr, uint16_t val );
        virtual void write_32( uint32_t addr, uint32_t val );

        bool loadData();
        void saveData();

        bool loadFile( QString file, eMcu* eMcu=nullptr );

        bool loadDat( QString file );

        QString getCsv();

        int wordBits() { return m_wordBits; }
        void setWordBits( int b );
        int wordBytes() { return m_wordBytes; }

        uint32_t sizeWords() { return m_data.size()/m_wordBytes; }
        uint32_t size() { return m_data.size(); }
        void resize( int size );

        uint8_t* rawData() { return m_rawData; }  // Get pointer to raw uint8_t array

        void fillMemory( uint32_t v );

        virtual void showTable();

        MemTable* getTable();

    protected:

        bool loadCsv( QString data );
        bool loadHex( QString file );
        bool loadBin( QString file );

        void saveCsv( QString file );
        void saveHex( QString file ); /// TODO
        void saveBin( QString file );

        int m_wordBits;
        int m_wordBytes;

        MemTable* m_memTable;
        eMcu* m_eMcu;

        std::vector<uint8_t> m_data;
        uint8_t* m_rawData;
};

#endif
