/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QMimeData>
#include <QDrag>

#include "listbase.h"
#include "treeitem.h"
#include "mainwindow.h"

ListBase::ListBase( QWidget* parent )
        : QTreeWidget( parent )
{
    m_searchFilter = "";

    setSelectionMode( QAbstractItemView::SingleSelection );
    setDragEnabled( true );
    viewport()->setAcceptDrops( true );
    setDropIndicatorShown( true );

    setIndentation( 12 );
    setRootIsDecorated( true );
    setCursor( Qt::OpenHandCursor );
    headerItem()->setHidden( true );

    float scale = MainWindow::self()->fontScale();
    setIconSize( QSize( 30*scale, 24*scale ));

    connect( this, &ListBase::itemPressed,
             this, &ListBase::slotItemClicked );
}
ListBase::~ListBase(){}

CompBase* ListBase::createItem( QString type, QString id )
{
    if( !m_componentFactory.contains( type ) ) return nullptr;

    listItem_t item = m_componentFactory.value( type );
    CompBase* comp = item.construct( id );
    comp->setItemType( item.type );
    return comp;
}

void ListBase::slotItemClicked( QTreeWidgetItem* item, int  )
{
    if( !item ) return;
    if( dragDropMode() == QAbstractItemView::InternalMove ) return; // Moving items in the list

    TreeItem* treeItem = (TreeItem*)item;
    QMimeData* mimeData = new QMimeData;
    mimeData->setText( treeItem->name()+","+treeItem->compType() );

    QDrag* drag = new QDrag( this );
    drag->setMimeData( mimeData );
    drag->exec( Qt::MoveAction, Qt::MoveAction );
}

void ListBase::addItem( QString caption, TreeItem* catItem, QString icon, QString type )
{
    QPixmap ic( icon );
    QIcon ico( ic );
    addItem( caption, catItem, ico, type );
}

void ListBase::addItem( listItem_t item )
{
    QString type = item.type;
    m_componentFactory[type] = item;

    QString icon = item.icon;
    QString iconFile = MainWindow::self()->getDataFilePath("images/"+icon );
    if( !QFile::exists( iconFile ) ) iconFile = ":/"+icon; // Image not in simulide data folder, use hardcoded image

    TreeItem* catItem = getCategory( item.category );
    if( catItem ) addItem( item.label, catItem, iconFile, type );
}

void ListBase::addItem( QString caption, TreeItem* catItem, QIcon &icon, QString type )
{
    if( !catItem ) return;

    QStringList nameFull = caption.split( "???" );
    QString       nameTr = nameFull.first();
    QString info = "";
    if( nameFull.size() > 1 ) info = "   "+nameFull.last();

    QString name = ( type == "Subcircuit" || type == "MCU" ) ? nameTr : type;

    /*if( type == "MCU" )
    {
        listItem_t libItem = Mcu::libraryItem();

        type = name;
        libItem.type = name;
        m_componentFactory[name] = libItem;
    }*/
    //else
    /*if( type == "Subcircuit" )
    {
        listItem_t libItem = SubCircuit::libraryItem();

        type = name;
        libItem.type = name;
        m_componentFactory[name] = libItem;
    }*/

    TreeItem* item = new TreeItem( catItem, name, nameTr, type, component, icon, m_customComp );

    item->setText( 0, nameTr+info );

    m_components.insert( name, item );

    //if( !m_restoreList )
        catItem->addChild( item );
}

TreeItem* ListBase::addCategory( QString nameTr, QString name, QString parent, QString icon )
{
    TreeItem* catItem = nullptr;
    TreeItem* catParent = nullptr;

    bool expanded = false;
    bool hidden   = false;

    if( parent.isEmpty() )                              // Is Main Category
    {
        catItem = new TreeItem( nullptr, name, nameTr, "", categ_MAIN, QIcon( QPixmap( icon ) )/*QIcon(":/null-0.png")*/, m_customComp );
        expanded = true;
    }else{
        if( m_categories.contains( parent ) ) catParent = m_categories.value( parent );
        catItem = new TreeItem( catParent, name, nameTr, "", categ_CHILD, QIcon( QPixmap( icon ) ), m_customComp );
    }

    //if( !m_restoreList )
    {
        if( parent.isEmpty() ) addTopLevelItem( catItem ); // Is root category or root category doesn't exist
        else if( catParent )   catParent->addChild( catItem );
    }
    catItem->setText( 0, nameTr );
    catItem->setItemHidden( hidden );
    catItem->setItemExpanded( expanded );
    m_categories.insert( name, catItem );
    m_catNames.insert( nameTr, name );

    return catItem;
}

TreeItem* ListBase::getCategory( QString category )
{
    QStringList catPath = category.split("/");
    QString parent = "";
    category = "";
    TreeItem* catItem = nullptr;

    while( !catPath.isEmpty() )
    {
        parent = category;
        category = catPath.takeFirst();
        if( category.isEmpty() ) continue;

        if( m_categories.contains( category ) ) catItem = m_categories.value( category );
        else{
            QString catTr = QObject::tr( category.toLocal8Bit() );
            catItem = addCategory( catTr, category, parent, "" );
        }
    }
    return catItem;
}

QString ListBase::getIcon( QString folder, QString name )
{
    QString icon = folder+"/"+name+"/"+name+"_icon.png";
    if( m_compSetDir.exists( icon ) ) icon = m_compSetDir.absoluteFilePath( icon );
    else                              icon = "";
    return icon;
}

void ListBase::search( QString filter )
{
    QList<QTreeWidgetItem*>    cList = findItems( filter, Qt::MatchContains|Qt::MatchRecursive, 0 );
    QList<QTreeWidgetItem*> allItems = findItems( "", Qt::MatchContains|Qt::MatchRecursive, 0 );

    for( QTreeWidgetItem* item : allItems )
    {
        TreeItem* treeItem = (TreeItem*)item;
        treeItem->setHidden( true );

        if( treeItem->childCount() > 0  )
        {
            if( m_searchFilter.isEmpty() )                            // First search, update actual expanded state
                treeItem->setItemExpanded( treeItem->isExpanded() );
            else treeItem->setExpanded( treeItem->isItemExpanded() ); // Don't setItemExpanded (keeps the original state)
            continue;
        }
        if( !cList.contains( item ) ) continue;

        bool hidden = treeItem->isItemHidden();
        while( treeItem ){
            if( hidden )
                treeItem->setHidden( hidden );
            treeItem->setHidden( hidden );
            if( treeItem->childCount() > 0 && !hidden /*&& !filter.isEmpty()*/ )
                treeItem->setExpanded( treeItem->isItemExpanded() );
            treeItem = treeItem->parentItem();
        }
    }
    m_searchFilter = filter;
}
