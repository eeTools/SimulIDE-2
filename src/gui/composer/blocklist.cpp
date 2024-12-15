/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QMimeData>
#include <QDrag>

#include "blocklist.h"
#include "mainwindow.h"

BlockList* BlockList::m_pSelf = NULL;

BlockList::BlockList( QWidget* parent )
         : QTreeWidget( parent )
{
    m_pSelf = this;

    setDragEnabled(true);
    setDragDropMode( QAbstractItemView::DragOnly );
    //setAlternatingRowColors(true);
    setIndentation( 12 );
    setRootIsDecorated( true );
    setCursor( Qt::OpenHandCursor );
    headerItem()->setHidden( true );

    float scale = MainWindow::self()->fontScale();
    setIconSize( QSize( 30*scale, 24*scale ));

    connect( this, &BlockList::itemPressed,
             this, &BlockList::slotItemClicked, Qt::UniqueConnection );

    registerItems();
}
BlockList::~BlockList(){}

Module* BlockList::createModule( QString type, QString id ) // Static
{
    if( !m_moduleFactory.contains( type ) ) return nullptr;

    Module* module = m_moduleFactory.value( type )(id);
    return module;
}

void BlockList::addItem( moduleItem_t moduleItem )
{
    m_moduleFactory[moduleItem.type] = moduleItem.construct;

    QTreeWidgetItem* item = new QTreeWidgetItem(0);
    float scale = MainWindow::self()->fontScale();
    QFont font;
    font.setFamily( MainWindow::self()->defaultFontName() );
    font.setBold( true );
    font.setPixelSize( 11*scale );

    item->setFlags( QFlag(32) );
    item->setFont( 0, font );
    item->setIcon( 0, QIcon( QPixmap( moduleItem.icon ) ) );
    item->setText( 0, moduleItem.label );
    item->setData( 0, Qt::UserRole, moduleItem.type );

    addTopLevelItem( item );
}

void BlockList::slotItemClicked( QTreeWidgetItem* item, int )
{
    if( !item ) return;

    QString type = item->data( 0, Qt::UserRole ).toString();
    if( type == "" ) return;

    QString name = item->data( 0, Qt::WhatsThisRole ).toString(); //item->text(0);
    QMimeData* mimeData = new QMimeData;
    mimeData->setText( name );
    mimeData->setHtml( type );              // Find a better solution

    QDrag* drag = new QDrag( this );
    drag->setMimeData( mimeData );
    drag->exec( Qt::CopyAction | Qt::MoveAction, Qt::CopyAction );
}


#include "m_bitop.h"
#include "m_abop.h"
#include "m_bittoint.h"
#include "m_inttobit.h"
#include "m_ioport.h"
#include "m_iohook.h"
#include "m_delay.h"
#include "m_property.h"

void BlockList::registerItems()
{
    addItem( mIoPort::registerItem() );
    addItem( IoHook::registerItem() );
    addItem( BitToInt::registerItem() );
    addItem( IntToBit::registerItem() );
    addItem( BitOp::registerItem() );
    addItem( AbOp::registerItem() );
    addItem( Delay::registerItem() );
    addItem( PropertyM::registerItem() );
}
