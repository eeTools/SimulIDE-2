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

ComponentList* ComponentList::m_pSelf = nullptr;

ComponentList::ComponentList( QWidget* parent )
             : ListBase( parent )
             , m_mcDialog( this )
{
    m_pSelf = this;


    m_mcDialog.setVisible( false );

    ///m_listFile  = MainWindow::self()->getConfigPath("compList.xml");
    ///m_restoreList = false; ///QFile::exists( m_listFile ); // Restore last List
    ///m_oldConfig = !m_restoreList; // xml file doesn't exist: read old config

    QString userDir = MainWindow::self()->userPath();
    if( !userDir.isEmpty() && QDir( userDir ).exists() ) LoadCompSetAt( userDir );

    ///if( !m_oldConfig ) readConfig(); // Read new xml config file

    m_customComp = false;
    registerItems();
    m_customComp = true;

    /*for( TreeItem* it : m_categories ) // Remove empty categories
    {
        if( it->childCount() ) continue;
        QTreeWidgetItem* pa = it->parent();
        if( pa ) pa->removeChild( it  );
    }*/

    setContextMenuPolicy( Qt::CustomContextMenu );

    connect( this, &ComponentList::customContextMenuRequested,
             this, &ComponentList::slotContextMenu );
}
ComponentList::~ComponentList(){}

void ComponentList::LoadCompSetAt( QDir compSetDir )
{
    m_compSetDir = compSetDir;



    QStringList compList = compSetDir.entryList( QDir::AllEntries | QDir::NoDotAndDotDot /*QDir::Files*/ );
    /// if( compList.isEmpty() ) return;                  // No comp sets to load

    qDebug() << "\n" << tr("    Loading Component sets at:")<< "\n" << compSetDir.absolutePath()<<"\n";

    for( QString comp : compList )
    {
        QString path = compSetDir.absoluteFilePath( comp );
        QFileInfo fileInfo = QFileInfo( path );

        if( fileInfo.isDir() ) LoadCompSetAt( path );
        else{
            if( fileInfo.suffix() != "comp") continue;

            QString doc = fileToString( path, "ComponentList::LoadCompSetAt" );
            QString line = doc.split("\n").takeFirst();

            QStringList tokens = line.split("; ");
            if( tokens.takeFirst() != "Component" ) continue;

            QString label;
            QString type;
            QString icon;
            TreeItem* categ = nullptr;

            for( QString prop : tokens )
            {
                QStringList p = prop.split("=");
                if( p.size() != 2 ) continue;
                if     ( p.first() == "icon"    ) icon  = p.last();
                else if( p.first() == "type"    ) type  = p.last();
                else if( p.first() == "label"   ) label = p.last();
                else if( p.first() == "category") categ = getCategory( p.last() );
            }

            if( categ )
            {
                addItem( label, categ, ":/"+icon, type );
               m_dataFileList.insert( type, path );
            }
        }
    }
    qDebug() << "\n";
    /// sortByColumn( 1, Qt::DescendingOrder );







    if( compSetDir.cd("Components") )
    {
        qDebug() << "\n" << tr("    Loading User Components at:")<< "\n" << compSetDir.absolutePath()+"/Components"<<"\n";
        loadComps( compSetDir );
        compSetDir.cd("..");
    }
    if( compSetDir.cd("test") )
    {
        QStringList dirList = compSetDir.entryList( {"*"}, QDir::Dirs );
        if( !dirList.isEmpty() )
        {
            TreeItem* catItem = getCategory("test");
            //if( !catItem ) catItem = addCategory("test","test","","" );

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

        QString category = attribs.value("category").toString();
        TreeItem* catItem = getCategory( category );

        /// TODO: reuse get category from catPath
        /*QString category = attribs.value("category").toString();
        QStringList catPath = category.split("/");

        TreeItem* catItem = nullptr;
        QString parent = "";
        category = "";
        while( !catPath.isEmpty() )
        {
            parent = category;
            category = catPath.takeFirst();
            catItem = getCategory( category );
            if( !catItem )
            {
                QString catTr = QObject::tr( category.toLocal8Bit() );
                catItem = addCategory( catTr, category, parent, "" );
            }
        }*/
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

            QString category = reader.attributes().value("category").toString();
            TreeItem* catItem = getCategory( category );
            /*QString catFull = reader.attributes().value("category").toString();
            //catFull.replace( "IC 74", "Logic/IC 74");
            QStringList catPath = catFull.split("/");

            TreeItem* catItem = nullptr;
            QString parent   = "";
            QString category = "";
            while( !catPath.isEmpty() )
            {
                parent = category;
                category = catPath.takeFirst();
                catItem = getCategory( category );
                if( !catItem )
                {
                    QString catTr = QObject::tr( category.toLocal8Bit() );
                    catItem = addCategory( catTr, category, parent, icon );
                }
            }*/

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

void ComponentList::mousePressEvent( QMouseEvent* event )
{
    if( event->modifiers() & Qt::ControlModifier ) setDragDropMode( QAbstractItemView::InternalMove );
    else                                           setDragDropMode( QAbstractItemView::DragOnly );

    for( QTreeWidgetItem* item : selectedItems() ) item->setSelected( false );
    QTreeWidget::mousePressEvent( event );
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

// --------------------------------------------
// BEGIN Item includes

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

void ComponentList::registerItems()
{
    addCategory( tr("Meters"),"Meters", "", "" );
    addItem( Probe::registerItem() );
    addItem( VoltMeter::registerItem() );
    addItem( AmpMeter::registerItem() );
    addItem( FreqMeter::registerItem() );
    addItem( Oscope::registerItem() );
    addItem( LAnalizer::registerItem() );

    addCategory( tr("Sources"),"Sources", "", "" );
    //addItem( FixedVolt::registerItem() );
    //addItem( Clock::registerItem() );
    //addItem( WaveGen::registerItem() );
    //addItem( VoltSource::registerItem() );
    //addItem( CurrSource::registerItem() );
    //addItem( Csource::registerItem() );
    //addItem( Battery::registerItem() );
    addItem( Rail::registerItem() );
    addItem( Ground::registerItem() );

    addCategory( tr("Switches"),"Switches", "", "" );
    //addItem( Push::registerItem() );
    //addItem( Switch::registerItem() );
    //addItem( SwitchDip::registerItem() );
    //addItem( Relay::registerItem() );
    //addItem( KeyPad::registerItem() );

    addCategory( tr("Passive"),"Passive", "", "" );
    addCategory( tr("Resistors"),"Resistors", "Passive", "resistors.png" );
    addItem( Resistor::registerItem() );
    //addItem( ResistorDip::registerItem() );
    //addItem( Potentiometer::registerItem() );
    //addItem( VarResistor::registerItem() );

    //addCategory( tr("Resistive Sensors"),"Resistive Sensors", "Passive", "resistorsensors.png" );
    //addItem( Ldr::registerItem() );
    //addItem( Thermistor::registerItem() );
    //addItem( RTD::registerItem() );
    //addItem( Strain::registerItem() );

    addCategory( tr("Reactive"),"Reactive", "Passive", "reactive.png" );
    addItem( Capacitor::registerItem() );
    //addItem( elCapacitor::registerItem() );
    addItem( Inductor::registerItem() );
    //addItem( Transformer::registerItem() );

    addCategory( tr("Active"),"Active", "", "" );
    addCategory( tr("Rectifiers"),"Rectifiers", "Active", "" );
    addItem( Diode::registerItem() );
    //addItem( Zener::registerItem() );
    //addItem( SCR::registerItem() );
    //addItem( Diac::registerItem() );
    //addItem( Triac::registerItem() );

    addCategory( tr("Transistors"),"Transistors", "Active", "" );
    addItem( BJT::registerItem() );
    addItem( Mosfet::registerItem() );

    addCategory( tr("Other Active"),"Other Active", "Active", "" );
    //addItem( OpAmp::registerItem() );
    //addItem( Comparator::registerItem() );
    //addItem( VoltReg::registerItem() );
    //addItem( MuxAnalog::registerItem() );

    //addCategory( tr("Outputs"),"Outputs", "", "" );
    //addCategory( tr("Leds"),"Leds", "Outputs", "" );
    //addItem( Led::registerItem() );
    //addItem( LedRgb::registerItem() );
    //addItem( LedBar::registerItem() );
    //addItem( SevenSegment::registerItem() );
    //addItem( LedMatrix::registerItem() );
    //addItem( Max72xx_matrix::registerItem() );
    //addItem( WS2812::registerItem() );

    //addCategory( tr("Displays"),"Displays", "Outputs", "" );
    //addItem( Hd44780::registerItem() );
    //addItem( Aip31068_i2c::registerItem() );
    //addItem( Pcd8544::registerItem() );
    //addItem( Ks0108::registerItem() );
    //addItem( Ssd1306::registerItem() );
    //addItem( Ili9341::registerItem() );

    //addCategory( tr("Motors"),"Motors", "Outputs", "" );
    //addItem( DcMotor::registerItem() );
    //addItem( Stepper::registerItem() );
    //addItem( Servo::registerItem() );

    //addCategory( tr("Other Outputs"),"Other Outputs", "Outputs", "" );
    //addItem( AudioOut::registerItem() );
    //addItem( Lamp::registerItem() );

    //addCategory( tr("Micro"),"Micro", "", "" );
    ////addItem( new LibraryItem( "AVR" , "Micro", "ic2.png","AVR", nullptr ) );
    ////addItem( new LibraryItem( "PIC" , "Micro", "ic2.png","PIC", nullptr ) );
    ////addItem( new LibraryItem( "I51" , "Micro", "ic2.png","I51", nullptr ) );
    ////addItem( new LibraryItem("MCS65", "Micro", "ic2.png","MCS65", nullptr ) );
    ////addItem( new LibraryItem("Z80"  , "Micro", "ic2.png","Z80", nullptr ) );
    ///// addItem( Mcu::registerItem() );
    ////addItem( new LibraryItem( QObject::tr("Arduino"), "Micro", "board.png","Arduino", nullptr ) );
    ////addItem( new LibraryItem( QObject::tr("Shields"), "Micro", "shield.png","Shields", nullptr ) );

    //addCategory( tr("Sensors"),"Sensors", "Micro", "1to2.png" );
    //addItem( SR04::registerItem() );
    //addItem( Dht22::registerItem() );
    //addItem( DS1621::registerItem() );
    //addItem( Ds18b20::registerItem() );

    //addCategory( tr("Peripherals"),"Peripherals", "Micro", "perif.png" );
    //addItem( SerialPort::registerItem() );
    //addItem( SerialTerm::registerItem() );
    //addItem( TouchPad::registerItem() );
    //addItem( KY023::registerItem() );
    //addItem( KY040::registerItem() );
    //addItem( DS1307::registerItem() );
    //addItem( Esp01::registerItem() );

    //addCategory( tr("Logic"),"Logic", "", "" );
    //addCategory( tr("Gates"),"Gates", "Logic", "gates.png" );
    //addItem( Buffer::registerItem() );
    //addItem( AndGate::registerItem() );
    //addItem( OrGate::registerItem() );
    //addItem( XorGate::registerItem() );

    //addCategory( tr("Arithmetic"),"Arithmetic", "Logic", "2to2.png" );
    //addItem( BinCounter::registerItem() );
    //addItem( FullAdder::registerItem() );
    //addItem( MagnitudeComp::registerItem() );
    //addItem( ShiftReg::registerItem() );
    //addItem( Function::registerItem() );

    //addCategory( tr("Memory"),"Memory", "Logic", "subc.png" );
    //addItem( FlipFlopD::registerItem() );
    //addItem( FlipFlopT::registerItem() );
    //addItem( FlipFlopRS::registerItem() );
    //addItem( FlipFlopJK::registerItem() );
    //addItem( LatchD::registerItem() );
    //addItem( SRAM::registerItem() );
    //addItem( DRAM::registerItem() );
    //addItem( I2CRam::registerItem() );

    //addCategory( tr("Converters"),"Converters", "Logic", "1to2.png" );
    //addItem( Mux::registerItem() );
    //addItem( Demux::registerItem() );
    //addItem( BcdToDec::registerItem() );
    //addItem( DecToBcd::registerItem() );
    //addItem( BcdTo7S::registerItem() );
    //addItem( I2CToParallel::registerItem() );

    //addCategory( tr("Other Logic"),"Other Logic", "Logic", "2to3.png" );
    //addItem( ADC::registerItem() );
    //addItem( DAC::registerItem() );
    //addItem( SevenSegmentBCD::registerItem() );
    //addItem( Lm555::registerItem() );

    ///// addItem( SubCircuit::registerItem() );
    //// Connectors
    addCategory( tr("Connectors"),"Connectors", "", "" );
    //addItem( Bus::registerItem() );
    addItem( Tunnel::registerItem() );
    addItem( Socket::registerItem() );
    addItem( Header::registerItem() );

    //addCategory( tr("Graphical"),"Graphical", "", "" );
    //addItem( Image::registerItem() );
    //addItem( TextComponent::registerItem() );
    //addItem( Rectangle::registerItem() );
    //addItem( Ellipse::registerItem() );
    //addItem( Line::registerItem() );

    addCategory( tr("Other"),"Other", "", "" );
    addItem( SubPackage::registerItem() );
    //addItem( TestUnit::registerItem() );
    //addItem( Dial::registerItem() );
}
