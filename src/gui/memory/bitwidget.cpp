/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "bitwidget.h"
#include "mainwindow.h"
//#include "e_mcu.h"
#include "watchable.h"
#include "utils.h"

BitWidget::BitWidget( QString name, QString type, Watchable* core, QWidget* parent )
         : QWidget( parent )
{
    setupUi(this);

    m_name = name;
    m_type = type.toLower();
    m_core = core;

    float scale = MainWindow::self()->fontScale();
    QFont fontS;
    fontS.setFamily("Ubuntu Mono");
    fontS.setBold( true );
    fontS.setPixelSize( round(12.5*scale) );

    QFont font;
    font.setFamily("Ubuntu Mono");
    font.setBold( true );
    font.setPixelSize( round(12.5*scale) );

    //nameLabel->setFont( font );
    //nameLabel->setText( m_name );

    bitTable->setFixedWidth( round(120*scale) );
    //bitTable->setReadOnly( true );
}

void BitWidget::updateValue()
{
    if( !m_core ) return;

    setValueInt( m_core->getIntReg( m_name ) );
}

void BitWidget::setValueInt( int val )
{
    for( int i=0; i<m_bits; ++i )
    {
        int bit = val & 1;
        if( bit ) bitTable->item( 0, m_bits-i )->setBackground( QColor( 255, 150, 00 ) );
        else      bitTable->item( 0, m_bits-i )->setBackground( QColor( 120, 230, 255 ) );
        val >>= 1;
    }
}

void BitWidget::setupTable()
{
    float scale = MainWindow::self()->fontScale()*0.8;
    int cellSize = round( 22*scale );
    int font_size = round(14*scale);
    //int numberColor = 0x202090;

    bitTable->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    bitTable->setRowHeight( 0, cellSize );
    bitTable->setFixedHeight( cellSize );

    QFont font;
    font.setFamily("Ubuntu Mono");
    font.setBold( true );
    font.setPixelSize( font_size );
    bitTable->setFont( font );

    if     ( m_type.endsWith("8")  ) m_bits = 8;  // 8 bits
    else if( m_type.endsWith("16") ) m_bits = 16; // 16 bits
    else if( m_type.endsWith("32") ) m_bits = 32; // 32 bits

    bitTable->setColumnCount( m_bits );

    font.setPixelSize( round(12*scale) );
    QTableWidgetItem* it;
    for( int i=0; i<m_bits; i++ )
    {
        it = new QTableWidgetItem(0);
        it->setFlags( Qt::ItemIsEnabled );
        it->setTextAlignment( Qt::AlignCenter );
        it->setFont( font );
        it->setText( m_bitNames.at(i) );
        bitTable->setItem( 0, i, it );
        bitTable->setColumnWidth( i, cellSize );
    }
}

void BitWidget::setBitNames( QStringList bits )
{
    m_bitNames = bits;
    while( m_bitNames.size() < m_bits ) m_bitNames.append("-");
    setupTable();
}
