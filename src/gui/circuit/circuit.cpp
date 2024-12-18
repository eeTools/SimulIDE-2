/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QSplitter>
#include <QMimeData>
#include <QSettings>

#include "circuit.h"
#include "simulator.h"
#include "mainwindow.h"
#include "circuitwidget.h"
#include "comproperty.h"
#include "componentlist.h"
#include "wireline.h"
#include "node.h"
#include "utils.h"
#include "subcircuit.h"
#include "subpackage.h"
//#include "mcu.h"
#include "simulator.h"
#include "linker.h"
#include "tunnel.h"
#include "createcomp.h"

Circuit* Circuit::m_pSelf = nullptr;

Circuit::Circuit( int width, int height, CircuitView* parent )
       : CanvasBase( width, height, parent )
{
    m_simulator = new Simulator();
    Tunnel::clearTunnels();

    setObjectName( "Circuit" );
    setParent( parent );
    m_graphicView = parent;

    m_pSelf = this;

    m_animate    = false;
    m_createSubc = false;
    m_acceptKeys = true;

    m_circRev = 1e6;          /// Fixme

    m_creCompDialog = nullptr;
    m_board = nullptr;
    m_subCircuit = nullptr;

    m_backupPath = MainWindow::self()->getConfigPath("backup.sim2");
    m_hideGrid   = MainWindow::self()->settings()->value( "Circuit/hideGrid" ).toBool();
    m_filePath   = "";

    connect( &m_bckpTimer, &QTimer::timeout,
                     this, &Circuit::saveBackup );

    qDebug() << endl << "-------------------------------------------------";
    qDebug() << "                   NEW CIRCUIT                   "<<endl;
}

Circuit::~Circuit()
{
    delete m_simulator;

    m_bckpTimer.stop();
    m_undoStack.clear();

    cancelComp();

    QFile file( m_backupPath );
    if( !file.exists() ) return;
    QFile::remove( m_backupPath ); // Remove backup file
}

Component* Circuit::getCompById( QString id )
{
    for( Component* comp : m_compList ) if( comp->getUid() == id ) return comp;
    return nullptr;
}

QString Circuit::getSeqNumber( QString name )
{
    QStringList words = name.split("-");
    if( words.size() > 1 ) return words.takeLast();
    return "";
}

void Circuit::loadCircuit( QString filePath )
{
    if( m_newWire ) return;

    m_busy = true;
    m_loading = true;
    m_filePath = filePath;
    m_error = 0;

    QString doc = fileToString( filePath, "Circuit::loadCircuit" );
    loadStrDoc( doc );

    m_busy = false;
    m_loading = false;

    if( m_error != 0 ) clearCanvas();
    else{
        m_graphicView->zoomToFit();
        qDebug() << "Circuit Loaded: ";
        qDebug() << filePath;
}   }

void Circuit::loadStrDoc( QString &doc )
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QList<Linker*>    linkList;   // Linked  Component list
    QList<Component*> compList;   // Pasting Component list
    QList<WireBase*>     connList;   // Pasting Connector list
    QList<Node*>      nodeList;   // Pasting node list

    m_circRev = 0;
    m_busy  = true;
    if( !m_undo && !m_redo ) m_LdPinMap.clear();

    QVector<QStringRef> docLines = doc.splitRef("\n");
    for( QStringRef line : docLines )
    {
        if( line.isEmpty() ) continue;
        QVector<propStr_t> properties = parseProps( line );

        propStr_t itemType = properties.takeFirst();
        QString type = itemType.name.toString();

        if( type == "circuit" )
        {
            if( m_pasting ) continue;

            for( propStr_t prop : properties ) // Set properties
            {
                if     ( prop.name == "stepSize") m_simulator->setStepSize( prop.value.toULongLong() );
                else if( prop.name == "stepsPS" ) m_simulator->setStepsPerSec(prop.value.toULongLong() );
                else if( prop.name == "NLsteps" ) m_simulator->setMaxNlSteps( prop.value.toUInt() );
                else if( prop.name == "reaStep" ) m_simulator->setReactStep( prop.value.toULongLong() );
                else if( prop.name == "animate" ) m_animate = prop.value.toInt();
                else if( prop.name == "width"   ) m_sceneWidth  = prop.value.toInt();
                else if( prop.name == "height"  ) m_sceneHeight = prop.value.toInt();
                else if( prop.name == "rev"     ) m_circRev  = prop.value.toInt();
            }
            continue;
        }

        if( type == "Wire" )
        {
            PinBase* startpin = nullptr;
            PinBase* endpin   = nullptr;
            QString startpinid, endpinid;
            QStringList pointList;

            QString uid;
            for( propStr_t prop : properties )
            {
                if     ( prop.name == "pin0" ) startpinid = prop.value.toString();
                else if( prop.name == "pin1" ) endpinid   = prop.value.toString();
                else if( prop.name == "pList") pointList  = prop.value.toString().split(",");
                else if( prop.name == "uid"  ) uid        = prop.value.toString();
            }
            if( m_pasting )
            {
                uid = ""; // force new Uid
                startpinid = replaceId( startpinid );
                endpinid   = replaceId( endpinid );
            }
            if( m_undo || m_redo )
            {
                startpin = m_pinMap.value( startpinid );
                endpin   = m_pinMap.value( endpinid );
            }else{
                startpin = m_LdPinMap.value( startpinid );
                endpin   = m_LdPinMap.value( endpinid );
            }

            if( !startpin && !m_pasting ) // Pin not found by name... find it by pos
            {
                int itemX = pointList.first().toInt();
                int itemY = pointList.at(1).toInt();
                startpin = findPin( itemX, itemY, startpinid );
            }
            if( !endpin && !m_pasting ) // Pin not found by name... find it by pos
            {
                int itemX = pointList.at( pointList.size()-2 ).toInt();
                int itemY = pointList.last().toInt();
                endpin = findPin( itemX, itemY, endpinid );
            }
            if( startpin && startpin->wire() ) startpin = nullptr;
            if( endpin   && endpin->wire()   ) endpin   = nullptr;

            if( startpin && endpin )    // Create Connector
            {
                if( uid.isEmpty() ) uid = newWireId();
                else{
                    int number = uid.split("-").last().toInt();
                    if( number > m_conNumber ) m_conNumber = number; // Adjust Connector counter: m_conNumber
                }
                Wire* wire = new Wire( uid, startpin, endpin );
                wire->setPointList( pointList );
                connList.append( wire );
            }
            else if( !m_pasting /*&& !m_undo && !m_redo*/ )// Start or End pin not found
            {
                if( !startpin ) qDebug() << "\n   ERROR!!  Circuit::loadStrDoc:  null startpin in Connector" << uid << startpinid;
                if( !endpin   ) qDebug() << "\n   ERROR!!  Circuit::loadStrDoc:  null endpin in Connector"   << uid << endpinid;
            }
        }
        else          // Component
        {
            propStr_t circId = properties.takeFirst();
            if( circId.name != "uid") continue; /// ERROR
            QString uid = circId.value.toString();
            QString newUid;

            if( m_pasting ) // Create new id
            {
                newUid = newSceneId();

                m_idMap[getSeqNumber( uid )] = newUid; // Map simu id to new id
            }
            else newUid = uid;

            if( type == "Node")
            {
                Node* joint = new Node( newUid );

                joint->loadProperties( properties );

                addItem( joint );
                nodeList.append( joint );
                m_compMap[newUid] = joint;
            }
            else{
                m_subCircuit = nullptr;  // Subcircuits set this value at
                Component* comp = createItem( type, newUid );

                if( !comp ){ qDebug() << " ERROR Creating Component: "<< type << uid;
                    continue; }

                /*if( type == "Subcircuit")
                {
                    lastComp  = comp;
                }*/
                /// Why?? // comp->setPropStr("label", label ); //setIdLabel( label );

                comp->loadProperties( properties );
                comp->setup();

                if( m_pasting ) comp->setIdLabel( newUid );
                comp->updtLabelPos();
                comp->updtValLabelPos();

                addItem( comp );
                if( type == "Package" ) compList.prepend( comp );
                else                    compList.append( comp );

                if( comp->m_isLinker ){
                    Linker* l = dynamic_cast<Linker*>(comp);
                    if( l->hasLinks() ) linkList.append( l );
                }
            }
            int number = newUid.split("-").last().toInt();
            if( number > m_seqNumber ) m_seqNumber = number; // Adjust item counter: m_seqNumber
        }


        /*if( line.contains("<mainCompProps") )
        {
            if( !lastComp ) continue;
            SubCircuit* subci = static_cast<SubCircuit*>(lastComp);
            Component*  mComp = subci->getMainComp();      // Old circuits with only 1 MainComp
            if( !mComp ) continue;

            for( propStr_t prop : properties )
            {
                if( prop.name == "MainCompId")  // If more than 1 mainComp then get Component
                {
                    QString compName = prop.value.toString();
                    mComp = subci->getMainComp( compName );
                    if( !mComp ) qDebug() << "ERROR: Could not get Main Component:"<< compName;
                }
                else mComp->setPropStr( prop.name.toString(), prop.value.toString() );
            }
        }*/

    }
    if( m_pasting )
    {
        m_idMap.clear();

        for( Component* comp : compList ){ comp->setSelected( true ); comp->move( m_deltaMove ); }
        for( Node*      node : nodeList ){ node->setSelected( true ); node->move( m_deltaMove ); }
        for( WireBase*  wire : connList ){ wire->select( true )     ; wire->move( m_deltaMove ); }
    }
    else for( Component* comp : compList ) { comp->moveSignal(); }

    m_nodeList += nodeList;
    m_wireList += connList;
    m_compList += compList;

    if( !m_undo && !m_redo ) // Take care about unconnected Joints
        for( Node* joint : nodeList ) joint->checkRemove(); // Only removed if some missing connector

    for( Linker*     linker : linkList   ) linker->createLinks( &compList );

    setAnimate( m_animate ); // Force Pin update
    setSize( m_sceneWidth, m_sceneHeight );

    m_busy = false;
    QApplication::restoreOverrideCursor();
    update();
}

void Circuit::createComp()
{
    if( m_creCompDialog ) return;
    m_creCompDialog = new creCompDialog( CircuitWidget::self() );
    CircuitWidget::self()->panelSplitter()->addWidget( m_creCompDialog );
    m_creCompDialog->show();
}

void Circuit::cancelComp()
{
    if( !m_creCompDialog ) return;
    m_creCompDialog->deleteLater();
    m_creCompDialog = nullptr;
}

QString Circuit::circuitHeader()
{
    QString header = "circuit";
    header += "; version=" +QString( APP_VERSION );
    header += "; rev="+QString( REVNO );
    header += "; stepSize="+QString::number( m_simulator->stepSize() );
    header += "; stepsPS=" +QString::number( m_simulator->stepsPerSec() );
    header += "; NLsteps=" +QString::number( m_simulator->maxNlSteps() );
    header += "; reaStep=" +QString::number( m_simulator->reactStep() );
    header += "; animate=" +QString::number( m_animate )+"\n";
    return header;
}

QString Circuit::circuitToString()
{
    if( m_board && m_board->m_boardMode ) m_board->setBoardMode( false );

    QString circuit = circuitHeader();
    for( Component* comp : m_compList ) circuit += comp->toString();
    for( Node*      node : m_nodeList ) circuit += node->toString();
    for( WireBase*  wire : m_wireList ) circuit += wire->toString();
    circuit += "\n";

    if( m_board && m_board->m_boardMode ) m_board->setBoardMode( true );
    return circuit;
}

bool Circuit::saveString( QString &fileName, QString doc )
{
    QFile file( fileName );
    if( !file.open( QFile::WriteOnly | QFile::Text ))
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning( nullptr, "Circuit::saveString",
        tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream out( &file );
    out.setCodec("UTF-8");
    out << doc;
    file.close();

    return true;
}

bool Circuit::saveCircuit( QString filePath )
{
    if( m_newWire ) return false;

    QApplication::setOverrideCursor( Qt::WaitCursor );

    QString oldFilePath = m_filePath;
    m_filePath = filePath;

    bool saved = false;
    if( filePath.endsWith(".subc") && m_creCompDialog ){
        saved = saveString( filePath, m_creCompDialog->toString() );
    }
    else saved = saveString( filePath, circuitToString() );

    if( saved ){
        qDebug() << "\nCircuit Saved: \n" << filePath;
        if( QFile::exists( m_backupPath ) ) QFile::remove( m_backupPath ); // Remove backup file
    }
    else m_filePath = oldFilePath;

    QApplication::restoreOverrideCursor();
    return saved;
}

void Circuit::importCircuit()
{
    if( m_newWire ) return;

    m_deltaMove = QPointF( 0, 0 );

    QString filePath = QFileDialog::getOpenFileName( 0l, tr("Import Circuit"), m_filePath,
                                          tr("Circuits (*.sim2);;All files (*.*)"));

    if( !filePath.endsWith(".sim2") ) return;

    QString doc = fileToString( filePath, "Circuit::importCirc" );
    QApplication::clipboard()->setText( doc );

    m_eventpoint = QPointF(0,0);
    paste( QPointF(0,0) );
}

Component* Circuit::createItem( QString type, QString id, bool map )
{
    CompBase* comp = ComponentList::self()->createItem( type, id );

    /// if( !comp ) comp = new fComponent( type, id );

    if( comp && map ) m_compMap[id] = comp;
    return (Component*)comp;
}

void Circuit::removeItems()                     // Remove Selected items
{
    if( m_newWire ) return;
    m_busy = true;
    if( m_simulator->isRunning() ) CircuitWidget::self()->powerCircOff();

    beginUndoStep(); // Save current state

    QList<Wire*> conns;
    QList<Component*> comps;

    for( QGraphicsItem* item : selectedItems() )    // Find all items to be removed
    {
        if( item->type() == QGraphicsItem::UserType+2 )      // ConnectorLine: add Connector to list
        {
            WireLine* line =  qgraphicsitem_cast<WireLine*>( item );
            Wire* wire = line->connector();
            if( !conns.contains( wire ) && m_oldWires.contains( wire ) ) conns.append( wire );
        }
        else if( item->type() == QGraphicsItem::UserType+1 ) // Component: add Component to list
        {
            Component* comp = qgraphicsitem_cast<Component*>( item );
            if( m_oldComps.contains( comp ) ) comps.append( comp );
        }
    }
    for( Wire* conn : conns ) removeWire( conn );         // Remove Connectors (does not delete)
    for( Component* comp: comps ){
        if( comp->itemType() == "Node" ) removeNode( (Node*)comp ); // Remove Nodes (does not delete)
        else                             removeComp( comp );        // Remove Components (does not delete)
    }
    endUndoStep();                                           // Calculates items actually created/removed

    for( QGraphicsItem* item : selectedItems() ) item->setSelected( false );
    m_busy = false;
}

void Circuit::removeComp( Component* comp )
{
    m_compRemoved = false;
    comp->remove();
    if( !m_compRemoved ) return;

    if( m_compList.contains( comp ) ) m_compList.removeOne( comp );
    removeItem( comp );
    m_compMap.remove( comp->getUid() );
    m_removedComps.append( comp );
}

void Circuit::clearCanvas() // Remove everything ( Clear Circuit )
{
    if( m_newWire ) return;
    m_deleting = true;
    if( m_simulator->isRunning() ) CircuitWidget::self()->powerCircOff();

    for( Component* comp : m_compList )
    {
        comp->remove();
        if( comp->scene() ) removeItem( comp );
        delete comp;
    }
    for( Node* node : m_nodeList )
    {
        if( node->scene() ) removeItem( node );
        delete node;
    }
    m_deleting = false;
}

void Circuit::saveBackup()
{
    if( m_simulator->isRunning() ) return;
    if( !m_changed || m_newWire || m_busy || m_deleting ) return;
    m_changed = false;

    m_busy = true;
    saveString( m_backupPath, circuitToString() ); // Backup file
    m_busy = false;
}

void Circuit::setChanged()
{
    m_changed = true;
    QString title = MainWindow::self()->windowTitle();
    if( !title.endsWith('*') ) MainWindow::self()->setWindowTitle(title+'*');
}

void Circuit::beginUndoStep() // Save current state
{
    beginCircuitBatch();

    m_oldWires = m_wireList;
    m_oldComps = m_compList;
    m_oldNodes = m_nodeList;
    m_compStrMap.clear();      /// TODO: optimize this, we are saving the whole circuit every time

    //save all comps
    for( WireBase*  wire : m_oldWires ) m_compStrMap.insert( wire, wire->toString() );
    for( Node*      node : m_oldNodes ) m_compStrMap.insert( node, node->toString() );
    for( Component* comp : m_oldComps ) m_compStrMap.insert( comp, comp->toString() );
}

void Circuit::calculateChanges()   // Calculates created/removed
{
    // Items Removed /// qDebug() << "Circuit::calcCicuitChanges Removed:";
    QList<WireBase*>  removedConns = substract( m_oldWires, m_wireList );
    QList<Node*>      removedNodes = substract( m_oldNodes, m_nodeList );
    QList<Component*> removedComps = substract( m_oldComps, m_compList );

    for( WireBase*  wire : removedConns ) addCompChange( wire->getUid(), COMP_STATE_NEW, m_compStrMap.value(wire) );
    for( Node*      node : removedNodes ) addCompChange( node->getUid(), COMP_STATE_NEW, m_compStrMap.value(node) );
    for( Component* comp : removedComps ) addCompChange( comp->getUid(), COMP_STATE_NEW, m_compStrMap.value(comp) );

    // Items Created /// qDebug() << "Circuit::calcCicuitChanges Created:";
    QList<WireBase*>  createdConns = substract( m_wireList, m_oldWires );
    QList<Node*>      createdNodes = substract( m_nodeList, m_oldNodes );
    QList<Component*> createdComps = substract( m_compList, m_oldComps );

    for( Component* comp : createdComps ) addCompChange( comp->getUid(), COMP_STATE_NEW, "" );
    for( Node*      node : createdNodes ) addCompChange( node->getUid(), COMP_STATE_NEW, "" );
    for( WireBase*  wire : createdConns ) addCompChange( wire->getUid(), COMP_STATE_NEW, "" );
}

void Circuit::restoreState()
{
    if( m_simulator->isRunning() ) CircuitWidget::self()->powerCircOff();
    m_busy = true;

    circChange& step = m_undoStack[ m_undoIndex ];

    int iStep, i;
    if( m_undo ) { iStep = -1; i = step.compChanges.size()-1; }
    else         { iStep =  1; i = 0; }

    while( i>=0 && i < step.compChanges.size() )
    {
        compChange* cChange = &step.compChanges[i];
        i += iStep;
        QString propName = cChange->property;
        QString propVal  = m_undo ? cChange->undoValue : cChange->redoValue;
        CompBase* comp   = m_compMap.value( cChange->component );             /// qDebug() << "Circuit::restoreState"<< cChange->component << propName << comp;

        if( propName == COMP_STATE_NEW )  // Create/Remove Item
        {
            if( propVal.isEmpty() )       // Remove item
            {
                if( !comp ) continue;
                if( m_undo && cChange->redoValue.isEmpty() ) cChange->redoValue = comp->toString();

                if     ( comp->itemType() == "Connector" ) removeWire( (Wire*)comp );
                else if( comp->itemType() == "Node"      ) removeNode( (Node*)comp );
                else                                       removeComp( (Component*)comp );
            }
            else loadStrDoc( propVal );   // Create Item
        }
        else if( comp )                   // Modify Property
        {
            if( m_undo && cChange->redoValue.isEmpty() ) cChange->redoValue = comp->getPropStr( propName );
            comp->setPropStr( propName, propVal );
        }
    }
    m_busy = false;
    deleteRemoved();                      // Delete Removed Components;
    for( WireBase* con : m_wireList ) {
        if( m_board && m_board->m_boardMode ) con->setVisib( false );
        else{
            con->startPin()->isMoved();
            con->endPin()->isMoved();
        }
    }
    update();
}

void Circuit::addComponent( Component* comp )
{
    addItem( comp );
    if( comp->itemType() == "Package" ) m_compList.prepend( comp );
    else                                m_compList.append( comp );
}

/*void Circuit::bom()
{
    if( m_newWire ) return;

    QString fileName = QFileDialog::getSaveFileName( MainWindow::self()
                            , tr( "Bill Of Materials" )
                            , changeExt( m_filePath, "-bom.txt" )
                            , "(*.*)"  );
    if( fileName.isEmpty() ) return;

    QStringList bom;
    for( Component* comp : m_compList ) bom.append( comp->print() );

    QFile file( fileName );
    if( !file.open(QFile::WriteOnly | QFile::Text) )
    {
          QMessageBox::warning(0l, "Circuit::bom",
          tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
    }
    bom.sort();

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out <<  "\nCircuit: ";
    out <<  QFileInfo( m_filePath ).fileName();
    out <<  "\n\n";
    out <<  "Bill of Materials:\n\n";
    for( QString line : bom ) out << line;

    file.close();
}*/

void Circuit::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    if( event->button() == Qt::LeftButton )
    {
        if( m_newWire ) event->accept();
        QGraphicsScene::mousePressEvent( event );

        if( !event->isAccepted() ) Linker::stopLinking(); // Click in empty place
    }
    else if( event->button() == Qt::RightButton )
    {
        if( m_newWire ) event->accept();
        else            QGraphicsScene::mousePressEvent( event );
    }
    else if( event->button() == Qt::MidButton )
        QGraphicsScene::mousePressEvent( event );
}

void Circuit::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
    if( event->button() == Qt::LeftButton )
    {
        if( m_newWire )  m_newWire->incActLine() ;
        QGraphicsScene::mouseReleaseEvent( event );
    }
    else if( event->button() == Qt::RightButton )
    {
        if( !m_newWire ) QGraphicsScene::mouseReleaseEvent( event );
}   }

void Circuit::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
    if( m_newWire )
    {
        event->accept();
        if( event->modifiers() & Qt::ShiftModifier) m_newWire->m_freeLine = true;
        m_newWire->updateConRoute( event->scenePos() );
    }
    QGraphicsScene::mouseMoveEvent( event );
}

void Circuit::keyPressEvent( QKeyEvent* event )
{
    if( !m_acceptKeys )  // Text Component
    {
        QGraphicsScene::keyPressEvent( event );
        return;
    }
    int key = event->key();

    if( m_newWire )
    {
        if( key == Qt::Key_Escape ) deleteNewWire();
        else QGraphicsScene::keyPressEvent( event );
        return;
    }
    if( key == Qt::Key_Escape )
    {
        Linker::stopLinking();
        return;
    }
    if( event->modifiers() & Qt::AltModifier ) // Create Component shortcut
    {
        QString str = event->text();
        if( str.isEmpty() )
        {
            QGraphicsScene::keyPressEvent( event );
            return;
        }
        QString compNam = ComponentList::self()->getComponent( str );
        if( !compNam.isEmpty() )
        {
            Component* enterItem = createItem( compNam, newSceneId() );
            if( enterItem )
            {
                QPoint cPos = QCursor::pos()-CircuitView::self()->mapToGlobal( QPoint(0,0));
                enterItem->setPos( toGrid( CircuitView::self()->mapToScene( cPos ) ) );
                addComponent( enterItem );
                saveCompChange( enterItem->getUid(), COMP_STATE_NEW, "" );
            }
        }
    }
    if( event->modifiers() & Qt::ControlModifier )
    {
        if( key == Qt::Key_C )
        {
            copy( QPointF(1e6,1e6) ); // We don't have an origin
            clearSelection();
        }
        if( key == Qt::Key_X )
        {
            QPoint p = CircuitWidget::self()->mapFromGlobal(QCursor::pos());
            copy( m_graphicView->mapToScene( p ) );
            removeItems();
        }
        else if( key == Qt::Key_V )
        {
            paste( QPointF(1e6,1e6) ); // We don't have a destination
        }
        else if( key == Qt::Key_Z )
        {
            if( event->modifiers() & Qt::ShiftModifier) redo();
            else undo();
        }
        else if( key == Qt::Key_Y ) redo();
        else if( key == Qt::Key_N ) CircuitWidget::self()->newCircuit();
        else if( key == Qt::Key_S )
        {
            if( event->modifiers() & Qt::ShiftModifier)
                 CircuitWidget::self()->saveCircAs();
            else CircuitWidget::self()->saveCirc();
        }
        else if( key == Qt::Key_O ) CircuitWidget::self()->openCirc();
        else if( key == Qt::Key_A )
        {
            for( Component* com : m_compList ) com->setSelected( true );
            for( Node*      nod : m_nodeList ) nod->setSelected( true );
            for( WireBase*     con : m_wireList ) con->select( true );
        }
        else if( key == Qt::Key_R )
        {
            if ( event->modifiers() & Qt::ShiftModifier ) {
                for( Component* com : m_compList ) if( com->isSelected() ) com->rotateCCW();
            } else {
                for( Component* com : m_compList ) if( com->isSelected() ) com->rotateCW();
            }
        }
        else if( key == Qt::Key_L )
        {
            if ( event->modifiers() & Qt::ShiftModifier ) {
                for( Component* com : m_compList ) if( com->isSelected() ) com->slotV_flip();
            } else {
                for( Component* com : m_compList ) if( com->isSelected() ) com->slotH_flip();
            }
        }
        else QGraphicsScene::keyPressEvent( event );
    }
    else if( key == Qt::Key_Delete || key == Qt::Key_Backspace )
    {
        removeItems();
        QGraphicsScene::keyPressEvent( event );
    } else {
        if( !event->isAutoRepeat() ) // Deliver Key events ( switches )
        {
            QString keys = event->text();
            while( keys.size() > 0 )
            {
                QString key = keys.left( 1 );
                keys.remove( 0, 1 );
                emit keyEvent( key, true );
        }  }
        QGraphicsScene::keyPressEvent( event );
}   }

void Circuit::keyReleaseEvent( QKeyEvent* event )
{
    if( !event->isAutoRepeat()
     && !( event->modifiers() & Qt::ControlModifier ) )  // Deliver Key events ( switches )
    {
        QString keys = event->text();
        while( keys.size() > 0 )
        {
            QString key = keys.left( 1 );
            keys.remove( 0, 1 );
            emit keyEvent( key, false );
    }   }
    QGraphicsScene::keyReleaseEvent( event );
}

void Circuit::dropEvent( QGraphicsSceneDragDropEvent* event )
{
    QString id   = event->mimeData()->text();
    QString file = "file://";
//qDebug() << "Circuit::dropEvent";

    if( !id.startsWith( file ) ) return;

    id.replace( file, "" ).replace("\r\n", "" ).replace("%20", " ");
#ifdef _WIN32
    if( id.startsWith( "/" )) id.remove( 0, 1 );
#endif
    QString loId = id.toLower();
    if( loId.endsWith( ".jpg") || loId.endsWith( ".png") || loId.endsWith( ".gif"))
    {
        file = id;
        Component* enterItem = createItem( "Image", newSceneId() );
        if( enterItem )
        {
            QPoint cPos = QCursor::pos()-CircuitView::self()->mapToGlobal( QPoint(0,0));
            enterItem->setPos( CircuitView::self()->mapToScene( cPos ) );
            enterItem->setBackground( file );
            addComponent( enterItem );
            saveCompChange( enterItem->getUid(), COMP_STATE_NEW, "" );
    }   }
    else CircuitWidget::self()->loadCirc( id );
}

WireBase* Circuit::newWire( QString id, PinBase* startPin, PinBase* endPin )
{
    Wire* wire = new Wire( id, startPin, endPin );
    QPoint p1 = startPin->scenePos().toPoint();
    wire->addConLine( p1.x(), p1.y(), p1.x(), p1.y(), 0 );
    return wire;
}

void Circuit::drawBackground( QPainter* painter, const QRectF &rect )
{
    /*painter->setBrush(QColor( 255, 255, 255 ) );
    painter->drawRect( m_scenerect );
    return;*/

    painter->fillRect( m_scenerect, QColor( 240, 240, 210 ) );
    painter->setPen( QColor( 210, 210, 210 ) );

    if( m_hideGrid ) return;

    int startx = m_scenerect.x();
    int endx   = m_scenerect.width()/2;
    int starty = m_scenerect.y();
    int endy   = m_scenerect.height()/2;

    int scnStrX = rect.x()-16;
    if( scnStrX < startx ) scnStrX = startx;

    int scnEndX = scnStrX+rect.width()+32;
    if( scnEndX > endx ) scnEndX = endx;

    int scnStrY = rect.y()-16;
    if( scnStrY < starty ) scnStrY = starty;

    int scnEndY = scnStrY+rect.height()+32;
    if( scnEndY > endy ) scnEndY = endy;

    for( int i=4; i<endx; i+=8 ){
        if( i > scnEndX && -i < scnStrX) break;
        if(  i < scnEndX ) painter->drawLine( i, scnStrY, i, scnEndY );
        if( -i > scnStrX ) painter->drawLine(-i, scnStrY,-i, scnEndY );
    }
    for( int i=4; i<endy; i+=8 ){
        if( i > scnEndY && -i < scnStrY) break;
        if(  i < scnEndY ) painter->drawLine( scnStrX, i, scnEndX, i);
        if( -i > scnStrY ) painter->drawLine( scnStrX,-i, scnEndX,-i);
    }
    QPen pen( QColor( 60, 60, 70 ), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    painter->setPen( pen );
    painter->setBrush( Qt::transparent );
    painter->drawRect( m_scenerect );
}

void Circuit::setSceneWidth( int w )
{
    m_sceneWidth = w;
    setSize( m_sceneWidth, m_sceneHeight );
}

void Circuit::setSceneHeight( int h )
{
    m_sceneHeight = h;
    setSize( m_sceneWidth, m_sceneHeight );
}

void Circuit::setDrawGrid( bool draw )
{
    m_hideGrid = !draw;
    if( m_hideGrid ) MainWindow::self()->settings()->setValue( "Circuit/hideGrid", "true" );
    else             MainWindow::self()->settings()->setValue( "Circuit/hideGrid", "false" );
    update();
}

void Circuit::setAnimate( bool an )
{
    m_animate = an;
    for( PinBase* pin : m_pinMap.values() ) pin->animate( an );
    update();
}

int Circuit::autoBck() { return MainWindow::self()->autoBck(); }
void Circuit::setAutoBck( int secs )
{
    m_bckpTimer.stop();
    if( secs < 1 ) secs = 0;
    else           m_bckpTimer.start( secs*1000 );

    MainWindow::self()->setAutoBck( secs );
}

#include "moc_circuit.cpp"
