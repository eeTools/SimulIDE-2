/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "valuewidget.h"
#include "mainwindow.h"
#include "watchable.h"
#include "utils.h"

ValueWidget::ValueWidget( QString name, QString type, Watchable* core, QWidget* parent )
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

    nameLabel->setFont( font );
    nameLabel->setText( m_name );

    typeLabel->setFont( fontS );
    typeLabel->setText( m_type );
    //typeLabel->setFixedWidth( round(40*scale) );

    valueLine->setFixedWidth( round(120*scale) );
    valueLine->setReadOnly( true );

    bitTable->setVisible( false );

    if     ( m_type.endsWith("8")  ) m_bits = 8;  // 8 bits
    else if( m_type.endsWith("16") ) m_bits = 16; // 16 bits
    else if( m_type.endsWith("32") ) m_bits = 32; // 32 bits
}

void ValueWidget::updateValue()
{
    if( !m_core ) return;

    if( m_type == "string" ) setValueStr( m_core->getStrReg( m_name ) );
    else                     setValueInt( m_core->getIntReg( m_name ) );
}

void ValueWidget::setValueInt( int val )
{
    if( m_bits == 8 )        // 8 bits
    {
        val = val  & 0xFF;
        QString decStr = decToBase( val, 10, 3 );
        QString hexStr = decToBase( val, 16, 2 );
        QString binStr = decToBase( val,  2, 8 );
        m_strVal = decStr+" 0x"+hexStr+" "+binStr;
    }
    else if( m_bits = 16 ) // 16 bits
    {
        val = val  & 0xFFFF;
        QString decStr = decToBase( val, 10, 5 );
        QString hexStr = decToBase( val, 16, 4 );
        m_strVal = decStr+"  0x"+hexStr;
    }
    else                            // Unknown
    {
        m_strVal = QString::number(val);
    }
    valueLine->setText( m_strVal );

    if( !m_bitNames.isEmpty()  )
    {
        for( int i=0; i<m_bits; ++i )
        {
            int bit = val & 1;
            if( bit ) bitTable->item( 0, m_bits-1-i )->setBackground( QColor( 255, 150, 00 ) );
            else      bitTable->item( 0, m_bits-1-i )->setBackground( QColor( 120, 230, 255 ) );
            val >>= 1;
        }
    }
}

void ValueWidget::setValueStr( QString str )
{
    valueLine->setText( str );
}

void ValueWidget::setupTable()
{
    float scale = MainWindow::self()->fontScale()*0.8;
    int cellHeight = round( 20*scale );
    int cellWidth = round( 36*scale );
    int fontSize = round(14*scale);
    //int numberColor = 0x202090;

    bitTable->setRowHeight( 0, cellHeight );
    bitTable->setFixedHeight( cellHeight );
    bitTable->setStyleSheet("QTableWidget::item { padding: 0px }");

    QFont font;
    font.setFamily("Ubuntu Mono");
    font.setBold( true );
    font.setPixelSize( fontSize );
    bitTable->setFont( font );
    bitTable->setColumnCount( m_bits );
    bitTable->setFixedWidth( cellWidth*m_bits );

    font.setPixelSize( round(11.5*scale) );
    QTableWidgetItem* it;
    for( int i=0; i<m_bits; i++ )
    {
        it = new QTableWidgetItem(0);
        it->setFlags( Qt::ItemIsEnabled );
        it->setTextAlignment( Qt::AlignCenter );
        it->setFont( font );
        it->setText( m_bitNames.at(i) );
        bitTable->setItem( 0, i, it );
        bitTable->setColumnWidth( i, cellWidth );
    }
}

void ValueWidget::setBitNames( QString bits )
{
    if( bits.isEmpty() ) return;
    m_bitNames = bits.split(",");
    while( m_bitNames.size() < m_bits ) m_bitNames.append("-");
    setupTable();

    bitTable->setVisible( true );
}
