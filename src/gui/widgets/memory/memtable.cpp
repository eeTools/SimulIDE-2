/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>
#include <QToolTip>
#include <QMenu>

#include "memtable.h"
#include "mainwindow.h"
#include "simulator.h"
#include "utils.h"
#include "memory.h"

MemTable::MemTable( QWidget* parent, Memory* memory, int wordBytes )
        : QWidget( parent )
{
    setupUi(this);

    m_memory = memory;
    m_data = &(m_memory->m_data);

    m_addrBytes = ceil( ceil(log2(m_data->size()))/8 );
    m_wordBytes = wordBytes;
    m_cellBytes = wordBytes;
    m_byteRatio = 1;
    m_updtCount = 0;

    m_hoverItem = nullptr;
    //m_canSaveLoad = true;

    resizeTable();

    table->setMouseTracking( true );

    setContextMenuPolicy( Qt::CustomContextMenu );

    connect( this, &MemTable::customContextMenuRequested, this, &MemTable::on_context_menu_requested );
    connect( actionSave_Memory_Table, &QAction::triggered, this, &MemTable::saveTable );
    connect( actionLoad_Memory_Table, &QAction::triggered, this, &MemTable::loadTable );
}

void MemTable::updateTable()
{
    if( ++m_updtCount >= 10 ) m_updtCount = 0;
    else                      return;
    updateData();
}

void MemTable::setValue( int address, int val )
{
    m_blocked = true;

    if( m_byteRatio > 1 )
    {
        address *= m_byteRatio;
        int mask = pow(2,8*m_cellBytes)-1;
        for( int i=0; i<m_byteRatio; ++i ) setCellValue( address+i, (val>>(i*8)) & mask );
    }
    else setCellValue( address, val );

    m_blocked = false;
}

void MemTable::setCellValue( int address, int val )
{
    int row = address/16;
    int colRam = address%16;
    int colAscii = colRam +17;

    QTableWidgetItem* item = table->item( row, colRam );
    if( underMouse() && item == m_hoverItem )
    {
        QString values = "Addr: 0x"+valToHex( address, m_addrBytes )
                       +"\nDec: "+decToBase( val, 10, 2*m_cellBytes+1 )
                       +"\nOct: "+decToBase( val,  8, 3*m_cellBytes )
                       +"\nBin: "+decToBase( val,  2, 8*m_cellBytes );

        if( QToolTip::text() != values )
            QToolTip::showText( QCursor::pos(), values, 0, {}, 100000 );
    }

    item->setData( 0, valToHex( val, m_cellBytes ) );
    QString valS = QChar( val&0x00FF );
    for( int i=1; i<m_cellBytes; ++i )
    {
        val >>= 8;
        valS.prepend( QString( QChar( val&0x00FF )) );
    }
    table->item( row, colAscii )->setData( 0, valS );
}

void MemTable::updateData()
{
    for( uint i=0; i<m_data->size(); ++i ) setValue( i, m_data->at(i) );
}

void MemTable::setCellBytes( int bytes )
{
    if( m_cellBytes == bytes ) return;

    m_cellBytes = bytes;
    m_byteRatio = m_wordBytes/m_cellBytes;
    resizeTable();
}

void MemTable::resizeTable()
{
    m_blocked = true;

    int dataSize = m_data->size();
    m_dataSize = dataSize;
    dataSize *= m_byteRatio;

    m_addrBytes = ceil( ceil(log2(dataSize))/8 );

    int rows = dataSize/16;
    if( dataSize%16) rows++;

    table->clear();

    //table->horizontalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    table->setHorizontalHeaderLabels( QStringList()
      <<"00"<<"01"<<"02"<<"03"<<"04"<<"05"<<"06"<<"07"
      <<"08"<<"09"<<"0A"<<"0B"<<"0C"<<"0D"<<"0E"<<"0F"
      << " "
      <<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"
      <<"8"<<"9"<<"A"<<"B"<<"C"<<"D"<<"E"<<"F");

    table->setRowCount( rows );

    float scale = MainWindow::self()->fontScale();
    QFont font;
    font.setFamily("Ubuntu Mono");
    font.setPixelSize( round(13*scale) );

    table->horizontalHeader()->setFont( font );
    table->verticalHeader()->setFont( font );

    QTableWidgetItem* it;

    for( int row=0; row<rows; ++row )
    {
        it = new QTableWidgetItem(0);
        it->setFlags( Qt::ItemIsEnabled );
        it->setFont( font );
        it->setText( " 0x"+valToHex( row*16, m_addrBytes )+" ");
        table->setVerticalHeaderItem( row, it );

        for( int col=0; col<33; ++col )
        {
            it = new QTableWidgetItem(0);
            if( col == 16 ) it->setFlags( 0 );
            else{
                if( col < 16 )
                {
                    font.setWeight( QFont::DemiBold );
                    it->setTextColor( QColor( 0x202090 ) );
                }
                else font.setWeight( QFont::Medium );
                it->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable );
                it->setFont( font );
                it->setTextAlignment( Qt::AlignHCenter );
            }
            table->setItem( row, col, it );
        }
        table->setRowHeight( row, 20*scale );
    }
    for( int col=0; col<16; ++col )
    {
        table->setColumnWidth( col, (18*m_cellBytes+5)*scale ); /// (2+m_cellBytes)*15*scale+2 );
        table->setColumnWidth( col+17, (8*m_cellBytes+4+4)*scale );
    }
    table->setColumnWidth( 16, 5 );

    m_blocked = false;
    updateData();
}

void MemTable::setAddrSelected( int addr, bool jump )
{
    if( addr >= m_dataSize ) return;
    int row = addr/16;
    int col = addr%16;
    cellClicked( row, col );
    if( jump ) table->scrollToItem( table->item( row, col ) );
}

void MemTable::on_table_itemChanged( QTableWidgetItem* item )
{
    if( m_blocked ) return;
    m_blocked = true;
    bool running = Simulator::self()->simState() > SIM_PAUSED;
    if( running ) Simulator::self()->pauseSim();

    int col = item->column();
    int row = item->row();
    int val=0;
    bool ok;

    if( col > 16 )
    {
        ok = item->text().size() == m_cellBytes;
        if( ok ){
            for( int i=0; i<m_cellBytes; ++i )
            {
                QChar cv = item->text().at(m_cellBytes-i-1);
                val += cv.cell() << (8*i);
            }
            col -= 17;
        }
    }
    else val = item->text().toInt( &ok, 0 );

    int tableAddr = row*16+col;
    int dataAddr = tableAddr/m_byteRatio;

    if( ok ) setCellValue( tableAddr, val );

    if( ok && (m_byteRatio > 1) )
    {
        int start = dataAddr*m_byteRatio;
        for( int i=0; i<m_byteRatio; ++i )
        {
            int addr = start+i;
            int row  = addr/16;
            int colRam  = addr%16;
            int cellVal = table->item( row, colRam )->text().toInt( &ok, 0 );
            val |= cellVal<<(i*8);
        }
    }

    if( ok )
    {
        if( m_data) m_data->at( dataAddr ) = val;
        emit dataChanged( dataAddr, val );
    }
    else if( m_data) setValue( dataAddr, m_data->at( dataAddr ) );

    if( running ) Simulator::self()->resumeSim();

    m_blocked = false;
}

void MemTable::on_table_itemEntered( QTableWidgetItem* item )
{
    m_hoverItem = item;
}

void MemTable::on_context_menu_requested( const QPoint &pos )
{
    //if( !m_canSaveLoad ) return;

    QMenu menu( this );

    menu.addAction( actionSave_Memory_Table );
    menu.addAction( actionLoad_Memory_Table );
    menu.exec( this->mapToGlobal( pos ) );
}

void MemTable::saveTable()
{
    m_memory->saveData();
}

void MemTable::loadTable()
{
    m_memory->loadData();
}

void MemTable::cellClicked( int row, int col )
{
    if( col == 16 ) return;

    if( col > 16 ) col -= 17;
    table->clearSelection();
    table->item( row, col )->setSelected( true );
    table->item( row, col+17 )->setSelected( true );
}

void MemTable::on_byteButton_toggled( bool byte )
{
    int bytes = byte ? 1 : m_wordBytes;
    setCellBytes( bytes );

    //updateStep();
}

void  MemTable::on_jumpButton_toggled( bool jump )
{
    //m_jumpToAddress = jump;
}

QString MemTable::valToHex( int val, int bytes )
{
    QString sval = QString::number( val, 16 ).toUpper();
    sval = sval.right( bytes*2 );
    while( sval.length() < bytes*2) sval.prepend( "0" );

    return sval;
}
