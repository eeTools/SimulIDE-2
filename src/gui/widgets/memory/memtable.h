/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MEMTABLE_H
#define MEMTABLE_H

#include <QWidget>

#include "ui_memtable.h"

class Memory;

class MemTable : public QWidget, private Ui::MemTable
{
    Q_OBJECT
    
    public:
        MemTable( QWidget* parent, Memory* memory, int wordBytes=1 );

        void resizeTable();
        void updateTable();
        void updateData();
        void setValue( int address, int val );
        void setCellBytes( int bytes );
        void setAddrSelected( int addr ,bool jump );

    signals:
        void dataChanged( int address, int val );

    public slots:
        void on_table_itemChanged( QTableWidgetItem* item );
        void on_table_cellClicked( int row, int col ) { cellClicked( row, col ); }
        void on_table_itemEntered( QTableWidgetItem* item );
        void on_context_menu_requested( const QPoint &pos );
        void on_byteButton_toggled( bool byte );
        void on_jumpButton_toggled( bool jump );
        void saveTable();
        void loadTable();

    private:
        void setCellValue( int address, int val );
        void cellClicked( int row, int col );
        QString valToHex( int val, int bytes );

        int m_updtCount;
        int m_dataSize;
        int m_wordBytes;
        int m_cellBytes;
        int m_byteRatio; // m_wordBytes/m_cellBytes
        int m_addrBytes;

        bool m_blocked;

        //bool m_canSaveLoad;

        QTableWidgetItem* m_hoverItem;

        Memory* m_memory;
        std::vector<uint8_t>* m_data;
};

#endif
