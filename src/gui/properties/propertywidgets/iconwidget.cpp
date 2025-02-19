/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>

#include "iconwidget.h"
#include "component.h"
#include "comproperty.h"
#include "propdialog.h"
#include "utils.h"

IconWidget::IconWidget( PropDialog* parent, ComProperty* prop )
          : PropWidget(  parent, prop )
{
    setupUi( this );
}
IconWidget::~IconWidget(){}

void IconWidget::setup( bool )
{
    QStringList iconList;
    iconList <<":/null_ico.png"<<":/subc_ico.png"<<":/subcl.png"<<":/subc2.png"<<":/ic2.png"
             <<":/perif.png"<<":/1to1.png"<<":/1to2.png"<<":/1to3.png"<<":/2to1.png"<<":/2to2.png"
             <<":/2to3.png"<<":/2to3g.png"<<":/3to1.png"<<":/3to2.png"<<":/3to2g.png"
             <<":/demux.png"<<":/mux.png"<<":/script_ico.png"<<":/shield.png"<<":/7segbcd.png";

    m_blocked = true;
    for( QString icon : iconList ) addIconFile( icon ); // iconBox->addItem( QIcon( icon ), "");
    m_blocked = false;
    iconBox->setCurrentIndex( 0 );

    this->adjustSize();
}

void IconWidget::on_iconBox_currentIndexChanged( QString )
{
    if( m_blocked ) return;

    int index = iconBox->currentIndex();
    QString iconData;
    if( index > 0 ) iconData = m_itemList.at( index ).iconData;

    m_property->setValStr( iconData );

    if( m_propDialog ) m_propDialog->changed();
}

void IconWidget::on_iconChoose_clicked()
{
    QString path = m_iconFile;
    if( path.isEmpty() ) path = m_circuitPath;

    QString iconFile = QFileDialog::getOpenFileName( 0l, tr("Select icon file"), path,
                                          tr("png Files (*.png);;All files (*.*)"));

    m_iconFile = iconFile;
    if( !iconFile.isEmpty() ) addIconFile( iconFile );
}

void IconWidget::addIconFile( QString iconFile )
{
    iconItem_t iconItem;
    iconItem.iconFile = iconFile;

    QByteArray ba = fileToByteArray( iconFile, "creCompDialog::addIconFile");
    QString iconData( ba.toHex() );
    iconItem.iconData = iconData;

    addIcon( iconItem );

    on_iconBox_currentIndexChanged("");
}

void IconWidget::addIcon( iconItem_t iconItem )
{
    m_itemList.append( iconItem );

    QByteArray ba;
    bool ok;
    for( int i=0; i<iconItem.iconData.size(); i+=2 )
    {
        QString ch = iconItem.iconData.mid( i, 2 );
        ba.append( ch.toInt( &ok, 16 ) );
    }
    QPixmap ic;
    ic.loadFromData( ba );
    QIcon ico( ic );

    iconBox->addItem( ico, "" );
    iconBox->setCurrentIndex( iconBox->count()-1 );
}

void IconWidget::updtValues()
{

}
