/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QApplication>
#include <QDomDocument>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QMimeData>
#include <QSettings>
#include <QDebug>
#include <QDrag>
#include <QMenu>
#include <QDir>

#include "componentlist.h"
#include "treeitem.h"
#include "mainwindow.h"
#include "circuit.h"
#include "circuitwidget.h"
#include "chip.h"
#include "utils.h"
#include "subcircuit.h"
/// #include "mcu.h"

ComponentList* ComponentList::m_pSelf = NULL;

ComponentList::ComponentList( QWidget* parent )
             : QTreeWidget( parent )
             , m_mcDialog( this )
{
    m_pSelf = this;

    m_searchFilter = "";

    m_mcDialog.setVisible( false );

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

    m_listFile  = MainWindow::self()->getConfigPath("compList.xml");
    m_restoreList = false; ///QFile::exists( m_listFile ); // Restore last List
    m_oldConfig = !m_restoreList; // xml file doesn't exist: read old config

    m_customComp = false;
    LoadLibraryItems();
    m_customComp = true;

    QString userDir = MainWindow::self()->userPath();
    if( !userDir.isEmpty() && QDir( userDir ).exists() ) LoadCompSetAt( userDir );

    if( !m_oldConfig ) readConfig(); // Read new xml config file

    /*for( TreeItem* it : m_categories ) // Remove empty categories
    {
        if( it->childCount() ) continue;
        QTreeWidgetItem* pa = it->parent();
        if( pa ) pa->removeChild( it  );
    }*/

    setContextMenuPolicy( Qt::CustomContextMenu );

    connect( this, &ComponentList::customContextMenuRequested,
             this, &ComponentList::slotContextMenu );

    connect( this, &ComponentList::itemPressed,
             this, &ComponentList::slotItemClicked );
}
ComponentList::~ComponentList(){}

void ComponentList::LoadCompSetAt( QDir compSetDir )
{
    m_compSetDir = compSetDir;

    if( compSetDir.cd("components") )
    {
        qDebug() << "\n" << tr("    Loading User Components at:")<< "\n" << compSetDir.absolutePath()+"/components"<<"\n";
        loadComps( compSetDir );
        compSetDir.cd("..");
    }
    if( compSetDir.cd("test") )
    {
        QStringList dirList = compSetDir.entryList( {"*"}, QDir::Dirs );
        if( !dirList.isEmpty() )
        {
            TreeItem* catItem = getCategory("test");
            if( !catItem ) catItem = addCategory("test","test","","" );

            for( QString compName : dirList )
            {
                QString path = compName+"/"+compName;
                QString icon = getIcon( "test", compName );
                QString compFile;
                QString type;

                if( compSetDir.exists( path+".sim2") )
                {
                    if( icon.isEmpty() ) icon = ":/subc.png";
                    type = "Subcircuit";
                    compFile = compSetDir.absoluteFilePath( path+".sim2" );
                }
                else if( compSetDir.exists( path+".mcu") )
                {
                    if( icon.isEmpty() ) icon = ":/ic2.png";
                    type = "MCU";
                }
                if( !type.isEmpty() && !m_components.contains( compName ) )
                {
                    addItem( compName, catItem, icon, type );
                    m_dirFileList[ compName ] = compSetDir.absoluteFilePath( compName );
                    if( !compFile.isEmpty() ) m_dataFileList[ compName ] = compFile;   // Save sim1 File used to create this item
                }
            }
        }
        compSetDir.cd("..");
    }
    compSetDir.setNameFilters( QStringList( "*.xml" ) );

    QStringList xmlList = compSetDir.entryList( QDir::Files );

    //if( xmlList.isEmpty() ) return;                  // No comp sets to load

    qDebug() << "\n" << tr("    Loading Component sets at:")<< "\n" << compSetDir.absolutePath()<<"\n";

    for( QString compSetName : xmlList )
    {
        QString compSetFilePath = compSetDir.absoluteFilePath( compSetName );
        if( !compSetFilePath.isEmpty() ) loadXml( compSetFilePath );
    }

    qDebug() << "\n";
}

void ComponentList::loadComps( QDir compSetDir )
{
    QStringList compList = compSetDir.entryList( {"*.subc"}, QDir::Files );

    for( QString compFile : compList )
    {
        compFile = compSetDir.absoluteFilePath( compFile );
        if( !compSetDir.exists( compFile ) ) continue;

        QFile file( compFile );
        if( !file.open(QFile::ReadOnly | QFile::Text) ){
              qDebug() << "ComponentList::loadComps Cannot read file"<< endl << compFile << endl << file.errorString();
              continue;
        }
        QFileInfo fi( compFile );
        QString compName = fi.baseName();

        QXmlStreamReader reader( &file );
        if( !reader.readNextStartElement() || reader.name() != "libitem" ){
            qDebug() << "ComponentList::loadComps Error parsing file (itemlib):"<< endl << compFile;
            file.close();
            continue;
        }
        QString icon = "";
        QByteArray ba;

        QXmlStreamAttributes attribs = reader.attributes();

        if( attribs.hasAttribute("icondata") )
        {
            QString icStr = attribs.value("icondata").toString();
            bool ok;
            for( int i=0; i<icStr.size(); i+=2 )
            {
                QString ch = icStr.mid( i, 2 );
                ba.append( ch.toInt( &ok, 16 ) );
            }
        }else{
            if( attribs.hasAttribute("icon") )
            {
                icon = attribs.value("icon").toString();
                if( !icon.startsWith(":/") )
                    icon = MainWindow::self()->getDataFilePath("images/"+icon);
            }
            else icon = getIcon("components", compName );
            if( !icon.isEmpty() ) ba = fileToByteArray( icon, "ComponentList::loadComps");
        }

        QPixmap ic;
        ic.loadFromData( ba );
        QIcon ico( ic );

        if( attribs.hasAttribute("compname") )
            compName = attribs.value("compname").toString();

        /// TODO: reuse get category from catPath
        QString category = attribs.value("category").toString();
        QStringList catPath = category.split("/");

        TreeItem* catItem = NULL;
        QString parent = "";
        category = "";
        while( !catPath.isEmpty() )
        {
            parent = category;
            category = catPath.takeFirst();
            catItem = getCategory( category );
            if( !catItem /*&& !parent.isEmpty()*/ )
            {
                QString catTr = QObject::tr( category.toLocal8Bit() );
                catItem = addCategory( catTr, category, parent, "" );
            }
        }
        QString type = attribs.value("itemtype").toString();

        if( !type.isEmpty() && !m_components.contains( compName ) )
        {
            m_dataFileList[ compName ] = compFile;   // Save comp File used to create this item

            if( attribs.hasAttribute("compinfo") )
                compName += "???"+attribs.value("compinfo").toString();

            addItem( compName, catItem, ico, type );
        }
    }

    QStringList dirList = compSetDir.entryList( {"*"}, QDir::Dirs );
    for( QString dir : dirList )
    {
        if( dir == "." || dir == "..") continue;
        if( !compSetDir.cd( dir )    ) continue;

        loadComps( compSetDir );
        compSetDir.cd( ".." );
    }
}

void ComponentList::loadXml( QString xmlFile )
{
    QFile file( xmlFile );
    if( !file.open(QFile::ReadOnly | QFile::Text) ){
          qDebug() << "ComponentList::loadXml Cannot read file"<< endl << xmlFile << endl << file.errorString();
          return;
    }
    QXmlStreamReader reader( &file );
    if( reader.readNextStartElement() )
    {
        if( reader.name() != "itemlib" ){
            qDebug() <<  "ComponentList::loadXml Error parsing file (itemlib):"<< endl << xmlFile;
            file.close();
            return;
        }
        while( reader.readNextStartElement() )
        {
            if( reader.name() != "itemset" ) { reader.skipCurrentElement(); continue;}

            QString icon = "";
            if( reader.attributes().hasAttribute("icon") )
            {
                icon = reader.attributes().value("icon").toString();
                if( !icon.startsWith(":/") )
                    icon = MainWindow::self()->getDataFilePath("images/"+icon);
            }

            QString catFull = reader.attributes().value("category").toString();
            //catFull.replace( "IC 74", "Logic/IC 74");
            QStringList catPath = catFull.split("/");

            TreeItem* catItem = NULL;
            QString parent   = "";
            QString category = "";
            while( !catPath.isEmpty() )
            {
                parent = category;
                category = catPath.takeFirst();
                catItem = getCategory( category );
                if( !catItem /*&& !parent.isEmpty()*/ )
                {
                    QString catTr = QObject::tr( category.toLocal8Bit() );
                    catItem = addCategory( catTr, category, parent, icon );
                }
            }

            QString type = reader.attributes().value("type").toString();
            QString folder = reader.attributes().value("folder").toString();

            while( reader.readNextStartElement() )
            {
                if( reader.name() == "item")
                {
                    QString name = reader.attributes().value("name").toString();

                    if( reader.attributes().hasAttribute("icon") )
                    {
                        icon = reader.attributes().value("icon").toString();
                        if( !icon.startsWith(":/") )
                            icon = MainWindow::self()->getDataFilePath("images/"+icon);
                    }
                    else icon = getIcon( folder, name );

                    if( catItem && !m_components.contains( name ) )
                    {
                        if( type == "Subcircuit" )
                        {
                            QString compFolder = QFileInfo( xmlFile ).absolutePath()+"/"+folder+"/"+name;
                            m_dirFileList[ name ] = compFolder;
                        }
                        m_dataFileList[ name ] = xmlFile;   // Save xml File used to create this item
                        if( reader.attributes().hasAttribute("info") )
                            name += "???"+reader.attributes().value("info").toString();

                        addItem( name, catItem, icon, type );
                    }
                    reader.skipCurrentElement();
    }   }   }   }
    QString compSetName = xmlFile.split( "/").last();

    qDebug() << tr("        Loaded Component set:           ") << compSetName;
}

QString ComponentList::getIcon( QString folder, QString name )
{
    QString icon = folder+"/"+name+"/"+name+"_icon.png";
    if( m_compSetDir.exists( icon ) ) icon = m_compSetDir.absoluteFilePath( icon );
    else                              icon = "";
    return icon;
}

Component* ComponentList::createComponent( QString type, QString id )
{
    if( !m_componentFactory.contains( type ) ) return nullptr;

    listItem_t item = m_componentFactory.value( type );
    Component* comp = item.construct( id );
    comp->setItemType( item.type );
    return comp;
}

void ComponentList::addItem( listItem_t item )
{
    QString type = item.type;
    m_componentFactory[type] = item;

    QString icon = item.icon;
    QString iconFile = MainWindow::self()->getDataFilePath("images/"+icon );
    if( !QFile::exists( iconFile ) ) iconFile = ":/"+icon; // Image not in simulide data folder, use hardcoded image

    TreeItem* catItem = getCategory( item.category );
    if( catItem ) addItem( item.label, catItem, iconFile, type );
}

void ComponentList::addItem( QString caption, TreeItem* catItem, QString icon, QString type )
{
    QPixmap ic( icon );
    QIcon ico( ic );
    addItem( caption, catItem, ico, type );
}

void ComponentList::addItem( QString caption, TreeItem* catItem, QIcon &icon, QString type )
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
    if( type == "Subcircuit" )
    {
        listItem_t libItem = SubCircuit::libraryItem();

        type = name;
        libItem.type = name;
        m_componentFactory[name] = libItem;
    }

    TreeItem* item = new TreeItem( catItem, name, nameTr, type, component, icon, m_customComp );

    item->setText( 0, nameTr+info );

    m_components.insert( name, item );

    if( !m_restoreList ) catItem->addChild( item );

    if( m_oldConfig )
    {
        bool hidden = MainWindow::self()->compSettings()->value( name+"/hidden" ).toBool();
        item->setItemHidden( hidden );

        QString shortCut = MainWindow::self()->compSettings()->value( name+"/shortcut" ).toString();
        item->setShortCut( shortCut );
        if( !shortCut.isEmpty() ) m_shortCuts[name] = shortCut;
    }
}

TreeItem* ComponentList::getCategory( QString category )
{
    TreeItem* catItem = NULL;
    if( m_categories.contains( category ) ) catItem = m_categories.value( category );
    else{
        category = m_catNames.value( category );
        if( !category.isEmpty() && m_categories.contains( category ) )
            catItem = m_categories.value( category );
    }
    return catItem;
}

TreeItem* ComponentList::addCategory( QString nameTr, QString name, QString parent, QString icon )
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

    if( !m_restoreList )
    {
        if( parent.isEmpty() ) addTopLevelItem( catItem ); // Is root category or root category doesn't exist
        else if( catParent )   catParent->addChild( catItem );
    }
    if( m_oldConfig )
    {
        if( MainWindow::self()->compSettings()->contains(name+"/collapsed") )
            expanded = !MainWindow::self()->compSettings()->value( name+"/collapsed" ).toBool();

        if( MainWindow::self()->compSettings()->contains(name+"/hidden") )
            hidden = MainWindow::self()->compSettings()->value( name+"/hidden" ).toBool();
    }
    catItem->setText( 0, nameTr );
    catItem->setItemHidden( hidden );
    catItem->setItemExpanded( expanded );
    m_categories.insert( name, catItem );
    m_catNames.insert( nameTr, name );

    return catItem;
}

void ComponentList::mousePressEvent( QMouseEvent* event )
{
    if( event->modifiers() & Qt::ControlModifier ) setDragDropMode( QAbstractItemView::InternalMove );
    else                                           setDragDropMode( QAbstractItemView::DragOnly );

    for( QTreeWidgetItem* item : selectedItems() ) item->setSelected( false );
    QTreeWidget::mousePressEvent( event );
}

void ComponentList::slotItemClicked( QTreeWidgetItem* item, int  )
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

void ComponentList::dropEvent( QDropEvent* event )
{
    QTreeWidget::dropEvent( event );

    for( TreeItem* catItem : m_categories )
    {
        if( catItem->parent() ) catItem->setItemType( categ_CHILD );
        else                    catItem->setItemType( categ_MAIN );
    }
}

void ComponentList::slotContextMenu( const QPoint& point )
{
    QMenu menu;

    QAction* manageComponents = menu.addAction( QIcon(":/fileopen.png"),tr("Manage Components") );
    connect( manageComponents, &QAction::triggered,
             this, &ComponentList::slotManageComponents, Qt::UniqueConnection );

    menu.exec( mapToGlobal(point) );
}

void ComponentList::slotManageComponents()
{
    m_mcDialog.initialize();
    m_mcDialog.setVisible( true );
}

void ComponentList::search( QString filter )
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

void ComponentList::readConfig()
{
    QDomDocument domDoc = fileToDomDoc( m_listFile, "ComponentList::insertItems" );
    if( domDoc.isNull() ) return;

    QDomElement root = domDoc.documentElement();
    QDomNode    tree = root.firstChild();
    readNodCfg( &domDoc, nullptr );               // Insert items as stored in file

    for( TreeItem* item : m_categories.values() ) // Insert new categories
    {
        TreeItem* parent = item->parentItem();
        if( parent ) parent->addChild( item );
        else         addTopLevelItem( item );
        item->setExpanded( item->isItemExpanded() );
    }

    for( TreeItem* item : m_components.values() ) // Insert new components
    {
        TreeItem* catItem = item->parentItem();
        if( catItem ) catItem->addChild( item );
    }
}

void ComponentList::readNodCfg( QDomNode* node, TreeItem* parent )
{
    TreeItem* item = nullptr;
    bool expanded = false;

    QDomElement element = node->toElement();
    QString name = element.attribute("name");

    if( element.tagName() == "category" )
    {
        item = m_categories.value( name );

        if( item ){
            m_categories.remove( name );
            expanded = element.attribute("expanded") == "1";

            treItemType_t itemType = parent ? categ_CHILD : categ_MAIN;
            item->setItemType( itemType );
        }
    }
    else if( element.tagName() == "component" )
    {
        item = m_components.value( name );

        if( item ){
            m_components.remove( name );
            QString shortcut = element.attribute("shortcut");
            item->setShortCut( shortcut );
            m_shortCuts.insert( shortcut, name );
        }
    }

    if( item ){
        if( m_restoreList ){
            if( parent ) parent->addChild( item );
            else         addTopLevelItem( item );
        }
        bool hidden = element.attribute("hidden") == "1";
        item->setItemHidden( hidden );
    }

    QDomNode child = node->firstChild(); // Recursively add items
    while( !child.isNull() ){
        readNodCfg( &child, item );
        child = child.nextSibling();
    }
    if( item ) item->setItemExpanded( expanded );
}

void ComponentList::writeSettings()
{
    search(""); // Exit from a posible search and update item states

    QString treeStr = "<comptree>\n";

    QList<QTreeWidgetItem*> topLevelList = ComponentList::self()->findItems("",Qt::MatchStartsWith);

    for( QTreeWidgetItem* catItem : topLevelList )
    {
        TreeItem* childItem = (TreeItem*)catItem;
        treeStr += childItem->toString("  ");
    }
    treeStr += "</comptree>\n";

    Circuit::self()->saveString( m_listFile, treeStr );
}

//BEGIN Item includes
#include "ampmeter.h"
#include "bjt.h"
#include "capacitor.h"
//#include "clock.h"
#include "diode.h"
//#include "fixedvolt.h"
#include "freqmeter.h"
#include "ground.h"
#include "header.h"
#include "inductor.h"
#include "logicanalizer.h"
#include "mosfet.h"
#include "oscope.h"
#include "probe.h"
#include "rail.h"
#include "resistor.h"
#include "socket.h"
#include "subcircuit.h"
#include "subpackage.h"
#include "tunnel.h"
#include "voltmeter.h"
//#include "wavegen.h"

/*
#include "adc.h"
#include "aip31068_i2c.h"
#include "audio_out.h"
#include "battery.h"
#include "bcdto7s.h"
#include "bcdtodec.h"
#include "bincounter.h"

#include "buffer.h"
#include "bus.h"
#include "capacitor.h"

#include "comparator.h"
#include "csource.h"
#include "currsource.h"
#include "dac.h"
#include "dcmotor.h"
#include "dectobcd.h"
#include "demux.h"
#include "dht22.h"
#include "diac.h"
#include "dial.h"

#include "ds1307.h"
#include "ds1621.h"
#include "ds18b20.h"
#include "dram.h"
#include "elcapacitor.h"
#include "ellipse.h"
#include "esp01.h"

#include "flipflopd.h"
#include "flipflopjk.h"
#include "flipfloprs.h"
#include "flipflopt.h"

#include "fulladder.h"
#include "function.h"
#include "gate_and.h"
#include "gate_or.h"
#include "gate_xor.h"

#include "hd44780.h"
#include "ili9341.h"
#include "image.h"
#include "i2cram.h"
#include "i2ctoparallel.h"

#include "keypad.h"
#include "ky023.h"
#include "ky040.h"
#include "ks0108.h"
#include "lamp.h"
#include "latchd.h"
#include "ldr.h"
#include "led.h"
#include "ledbar.h"
#include "ledmatrix.h"
#include "ledrgb.h"
#include "line.h"
#include "lm555.h"

#include "magnitudecomp.h"
#include "max72xx_matrix.h"
#include "mux.h"
#include "mux_analog.h"
#include "op_amp.h"
#include "pcd8544.h"
#include "potentiometer.h"
#include "push.h"
#include "rectangle.h"
#include "relay.h"
#include "resistordip.h"
#include "rtd.h"
#include "serialport.h"
#include "serialterm.h"
#include "servo.h"
#include "sevensegment.h"
#include "sevensegment_bcd.h"
#include "shiftreg.h"

#include "sr04.h"
#include "sram.h"
#include "scr.h"
#include "ssd1306.h"
#include "stepper.h"
#include "strain.h"

#include "switch.h"
#include "switchdip.h"
#include "thermistor.h"
#include "testunit.h"
#include "textcomponent.h"
#include "touchpad.h"
#include "transformer.h"
#include "triac.h"
#include "varresistor.h"

#include "volt_reg.h"
#include "voltsource.h"
#include "ws2812.h"
#include "zener.h"*/

void ComponentList::LoadLibraryItems()
{
    addCategory( tr("Meters"),"Meters", "", "" );
    addItem( Probe::libraryItem() );
    addItem( VoltMeter::libraryItem() );
    addItem( AmpMeter::libraryItem() );
    addItem( FreqMeter::libraryItem() );
    addItem( Oscope::libraryItem() );
    addItem( LAnalizer::libraryItem() );

    addCategory( tr("Sources"),"Sources", "", "" );
    //addItem( FixedVolt::libraryItem() );
    //addItem( Clock::libraryItem() );
    //addItem( WaveGen::libraryItem() );
    //addItem( VoltSource::libraryItem() );
    //addItem( CurrSource::libraryItem() );
    //addItem( Csource::libraryItem() );
    //addItem( Battery::libraryItem() );
    addItem( Rail::libraryItem() );
    addItem( Ground::libraryItem() );

    addCategory( tr("Switches"),"Switches", "", "" );
    //addItem( Push::libraryItem() );
    //addItem( Switch::libraryItem() );
    //addItem( SwitchDip::libraryItem() );
    //addItem( Relay::libraryItem() );
    //addItem( KeyPad::libraryItem() );

    addCategory( tr("Passive"),"Passive", "", "" );
    addCategory( tr("Resistors"),"Resistors", "Passive", "resistors.png" );
    addItem( Resistor::libraryItem() );
    //addItem( ResistorDip::libraryItem() );
    //addItem( Potentiometer::libraryItem() );
    //addItem( VarResistor::libraryItem() );

    //addCategory( tr("Resistive Sensors"),"Resistive Sensors", "Passive", "resistorsensors.png" );
    //addItem( Ldr::libraryItem() );
    //addItem( Thermistor::libraryItem() );
    //addItem( RTD::libraryItem() );
    //addItem( Strain::libraryItem() );

    addCategory( tr("Reactive"),"Reactive", "Passive", "reactive.png" );
    addItem( Capacitor::libraryItem() );
    //addItem( elCapacitor::libraryItem() );
    addItem( Inductor::libraryItem() );
    //addItem( Transformer::libraryItem() );

    addCategory( tr("Active"),"Active", "", "" );
    addCategory( tr("Rectifiers"),"Rectifiers", "Active", "" );
    addItem( Diode::libraryItem() );
    //addItem( Zener::libraryItem() );
    //addItem( SCR::libraryItem() );
    //addItem( Diac::libraryItem() );
    //addItem( Triac::libraryItem() );

    addCategory( tr("Transistors"),"Transistors", "Active", "" );
    addItem( BJT::libraryItem() );
    addItem( Mosfet::libraryItem() );

    addCategory( tr("Other Active"),"Other Active", "Active", "" );
    //addItem( OpAmp::libraryItem() );
    //addItem( Comparator::libraryItem() );
    //addItem( VoltReg::libraryItem() );
    //addItem( MuxAnalog::libraryItem() );

    //addCategory( tr("Outputs"),"Outputs", "", "" );
    //addCategory( tr("Leds"),"Leds", "Outputs", "" );
    //addItem( Led::libraryItem() );
    //addItem( LedRgb::libraryItem() );
    //addItem( LedBar::libraryItem() );
    //addItem( SevenSegment::libraryItem() );
    //addItem( LedMatrix::libraryItem() );
    //addItem( Max72xx_matrix::libraryItem() );
    //addItem( WS2812::libraryItem() );

    //addCategory( tr("Displays"),"Displays", "Outputs", "" );
    //addItem( Hd44780::libraryItem() );
    //addItem( Aip31068_i2c::libraryItem() );
    //addItem( Pcd8544::libraryItem() );
    //addItem( Ks0108::libraryItem() );
    //addItem( Ssd1306::libraryItem() );
    //addItem( Ili9341::libraryItem() );

    //addCategory( tr("Motors"),"Motors", "Outputs", "" );
    //addItem( DcMotor::libraryItem() );
    //addItem( Stepper::libraryItem() );
    //addItem( Servo::libraryItem() );

    //addCategory( tr("Other Outputs"),"Other Outputs", "Outputs", "" );
    //addItem( AudioOut::libraryItem() );
    //addItem( Lamp::libraryItem() );

    //addCategory( tr("Micro"),"Micro", "", "" );
    ////addItem( new LibraryItem( "AVR" , "Micro", "ic2.png","AVR", NULL ) );
    ////addItem( new LibraryItem( "PIC" , "Micro", "ic2.png","PIC", NULL ) );
    ////addItem( new LibraryItem( "I51" , "Micro", "ic2.png","I51", NULL ) );
    ////addItem( new LibraryItem("MCS65", "Micro", "ic2.png","MCS65", NULL ) );
    ////addItem( new LibraryItem("Z80"  , "Micro", "ic2.png","Z80", NULL ) );
    ///// addItem( Mcu::libraryItem() );
    ////addItem( new LibraryItem( QObject::tr("Arduino"), "Micro", "board.png","Arduino", NULL ) );
    ////addItem( new LibraryItem( QObject::tr("Shields"), "Micro", "shield.png","Shields", NULL ) );

    //addCategory( tr("Sensors"),"Sensors", "Micro", "1to2.png" );
    //addItem( SR04::libraryItem() );
    //addItem( Dht22::libraryItem() );
    //addItem( DS1621::libraryItem() );
    //addItem( Ds18b20::libraryItem() );

    //addCategory( tr("Peripherals"),"Peripherals", "Micro", "perif.png" );
    //addItem( SerialPort::libraryItem() );
    //addItem( SerialTerm::libraryItem() );
    //addItem( TouchPad::libraryItem() );
    //addItem( KY023::libraryItem() );
    //addItem( KY040::libraryItem() );
    //addItem( DS1307::libraryItem() );
    //addItem( Esp01::libraryItem() );

    //addCategory( tr("Logic"),"Logic", "", "" );
    //addCategory( tr("Gates"),"Gates", "Logic", "gates.png" );
    //addItem( Buffer::libraryItem() );
    //addItem( AndGate::libraryItem() );
    //addItem( OrGate::libraryItem() );
    //addItem( XorGate::libraryItem() );

    //addCategory( tr("Arithmetic"),"Arithmetic", "Logic", "2to2.png" );
    //addItem( BinCounter::libraryItem() );
    //addItem( FullAdder::libraryItem() );
    //addItem( MagnitudeComp::libraryItem() );
    //addItem( ShiftReg::libraryItem() );
    //addItem( Function::libraryItem() );

    //addCategory( tr("Memory"),"Memory", "Logic", "subc.png" );
    //addItem( FlipFlopD::libraryItem() );
    //addItem( FlipFlopT::libraryItem() );
    //addItem( FlipFlopRS::libraryItem() );
    //addItem( FlipFlopJK::libraryItem() );
    //addItem( LatchD::libraryItem() );
    //addItem( SRAM::libraryItem() );
    //addItem( DRAM::libraryItem() );
    //addItem( I2CRam::libraryItem() );

    //addCategory( tr("Converters"),"Converters", "Logic", "1to2.png" );
    //addItem( Mux::libraryItem() );
    //addItem( Demux::libraryItem() );
    //addItem( BcdToDec::libraryItem() );
    //addItem( DecToBcd::libraryItem() );
    //addItem( BcdTo7S::libraryItem() );
    //addItem( I2CToParallel::libraryItem() );

    //addCategory( tr("Other Logic"),"Other Logic", "Logic", "2to3.png" );
    //addItem( ADC::libraryItem() );
    //addItem( DAC::libraryItem() );
    //addItem( SevenSegmentBCD::libraryItem() );
    //addItem( Lm555::libraryItem() );

    ///// addItem( SubCircuit::libraryItem() );
    //// Connectors
    addCategory( tr("Connectors"),"Connectors", "", "" );
    //addItem( Bus::libraryItem() );
    addItem( Tunnel::libraryItem() );
    addItem( Socket::libraryItem() );
    addItem( Header::libraryItem() );

    //addCategory( tr("Graphical"),"Graphical", "", "" );
    //addItem( Image::libraryItem() );
    //addItem( TextComponent::libraryItem() );
    //addItem( Rectangle::libraryItem() );
    //addItem( Ellipse::libraryItem() );
    //addItem( Line::libraryItem() );

    addCategory( tr("Other"),"Other", "", "" );
    addItem( SubPackage::libraryItem() );
    //addItem( TestUnit::libraryItem() );
    //addItem( Dial::libraryItem() );
}
