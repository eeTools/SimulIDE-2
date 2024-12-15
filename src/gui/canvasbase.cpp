/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QGraphicsView>
#include <QApplication>
#include <QClipboard>
#include <QTextStream>
#include <QFileDialog>
#include <QDebug>

#include "canvasbase.h"
#include "mainwindow.h"
#include "node.h"
#include "utils.h"

CanvasBase::CanvasBase( qreal x, qreal y, qreal w, qreal h, QGraphicsView* parent )
          : QGraphicsScene( x, y, w, h, parent )
{
    setParent( parent );
    setSceneRect( QRectF(x, y, w, h) );

    m_newWire = nullptr;

    m_busy     = false;
    m_undo     = false;
    m_redo     = false;
    m_changed  = false;
    m_pasting  = false;
    m_deleting = false;
    m_loading  = false;

    m_filePath = "";

    m_undoIndex = -1;
    m_maxUndoSteps = 100;
    m_cicuitBatch = 0;

    m_seqNumber = 0;
    m_conNumber = 0;
}
CanvasBase::~CanvasBase()
{
    m_undoStack.clear();
}

void CanvasBase::clearCanvas()
{
    if( m_newWire ) return;
    m_deleting = true;

    for( Node* node : m_nodeList )
    {
        if( node->scene() ) removeItem( node );
        delete node;
    }
    m_nodeList.clear();
    m_wireList.clear();
    m_pinMap.clear();
    m_LdPinMap.clear();
    m_idMap.clear();
    m_compMap.clear();
    m_deleting = false;
}

Wire* CanvasBase::createWire( QList<prop_t> properties, QString newUid )
{
    Wire* wire = nullptr;
    PinBase* startpin = nullptr;
    PinBase* endpin   = nullptr;
    QString startpinid, endpinid;
    QStringList pointList;

    for( prop_t prop : properties )
    {
        QString name = prop.name;
        QString val  = prop.value;

        if     ( name == "pin0" ) startpinid = val;
        else if( name == "pin1" ) endpinid   = val;
        else if( name == "pList") pointList  = val.split(",");
    }
    if( m_pasting )
    {
        newUid     = newWireId();
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
    if( !m_pasting )
    {
        if( !startpin  ) // Pin not found by name... find it by pos
        {
            int itemX = pointList.first().toInt();
            int itemY = pointList.at(1).toInt();
            startpin  = findPin( itemX, itemY, startpinid );
        }
        if( !endpin )   // Pin not found by name... find it by pos
        {
            int itemX = pointList.at( pointList.size()-2 ).toInt();
            int itemY = pointList.last().toInt();
            endpin    = findPin( itemX, itemY, endpinid );
        }
    }
    if( startpin && startpin->wire() ) startpin = nullptr;
    if( endpin   && endpin->wire()   ) endpin   = nullptr;

    if( startpin && endpin )    // Create Wire
    {
        if( newUid.isEmpty() ) newUid = newWireId();
        else{
            int number = newUid.toInt();
            if( number > m_conNumber ) m_conNumber = number; // Adjust Wire counter: m_conNumber
        }
        wire = new Wire( newUid, startpin, endpin );
        wire->setPointList( pointList );
    }
    else if( !m_pasting /*&& !m_undo && !m_redo*/ )// Start or End pin not found
    {
        if( !startpin ) qDebug() << "\n   ERROR!!  Circuit::loadStrDoc:  null startpin in Wire" << newUid << startpinid;
        if( !endpin   ) qDebug() << "\n   ERROR!!  Circuit::loadStrDoc:  null endpin in Wire"   << newUid << endpinid;
    }
    return wire;
}

bool CanvasBase::saveStrDoc( QString fileName, QString doc )
{
    QFile file( fileName );
    if( !file.open( QFile::WriteOnly | QFile::Text ))
    {
        qDebug() << "CanvasBase::saveStrDoc" << tr("Cannot write file %1:\n%2.").arg(fileName).arg( file.errorString() );
        return false;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << doc;
    file.close();

    return true;
}

QString CanvasBase::replaceId( QString pinName )
{
    QStringList words = pinName.split("@");
    words.replace( 1, m_idMap.value( words.at( 1 ) ) ); // Replace component uid
    return words.join("@");
}

PinBase* CanvasBase::findPin( int x, int y, QString id )
{
    // qDebug() << "Circuit::findPin" << id;
    QRectF itemRect = QRectF ( x-4, y-4, 8, 8 );

/// TODO: optimize

    QList<QGraphicsItem*> list = items( itemRect ); // List of items in (x, y)
    for( QGraphicsItem* it : list )
    {
        if( it->type() == QGraphicsItem::UserType + 3
         || it->type() == QGraphicsItem::UserType + 4  )
        {
            PinBase* pin =  qgraphicsitem_cast<PinBase*>( it );
            if( pin && pin->pinId().left(1) == id.left(1) && !pin->wire() ) return pin; // Check if names start by same letter
        }
    }
    for( QGraphicsItem* it : list ) // Not found by first letter, take first Pin
    {
        if( it->type() == QGraphicsItem::UserType + 3
         || it->type() == QGraphicsItem::UserType + 4  )
        {
            PinBase* pin =  qgraphicsitem_cast<PinBase*>( it );
            if( pin && !pin->wire() ) return pin;
        }
    }
    return NULL;
}

void CanvasBase::addNode( Node* node )
{
    addItem( node );
    m_nodeList.insert( node );
    m_compMap.insert( node->getUid(), node );
}

void CanvasBase::newWire( PinBase* startpin, bool save )
{
    if( save ) beginUndoStep();

    m_newWire = new Wire( newWireId(), startpin );
    m_wireList.insert( m_newWire );
}

void CanvasBase::closeWire( PinBase* endpin, bool save )
{
    if( !m_newWire ) return;
    m_newWire->closeCon( endpin );
    m_newWire = nullptr;
    if( save ) endUndoStep();
}

void CanvasBase::deleteNewWire()
{
    if( !m_newWire ) return;
    removeWire( m_newWire );
    m_newWire = nullptr;
    cancelUndoStep();
    update();
}

void CanvasBase::removeWire( Wire* wire )
{
    if( !m_wireList.contains(wire) ) return;
    wire->remove();
    removeItem( wire );
    m_wireList.remove( wire );
    m_compMap.remove( wire->getUid() );
    m_removedComps.insert( wire );
}

void CanvasBase::removeNode( Node* node )
{
    if( !m_nodeList.contains(node) ) return;
    if( m_deleting ) return;
    m_nodeList.remove( node );
    m_compMap.remove( node->getUid() );
    removeItem( node );
    m_removedComps.insert( node );
}

void CanvasBase::importCircuit()
{
    if( m_newWire ) return;

    m_deltaMove = QPointF( 0, 0 );

    QString filePath = QFileDialog::getOpenFileName( 0l, tr("Import Circuit"), m_filePath,
                                          tr("All files (*.*)"));

    if( !filePath.isEmpty()
     && ( filePath.endsWith(".simu") || filePath.endsWith(".sim1") ) )
    {
        QString doc = fileToString( filePath, "CanvasBase::importCirc" );
        QApplication::clipboard()->setText( doc );

        m_eventpoint = QPointF(0,0);
        paste( QPointF(0,0) );
    }
}

void CanvasBase::copy( QPointF eventpoint )
{
    if( m_newWire ) return;

    m_eventpoint = toGrid( eventpoint );

    QList<CompBase*> complist;
    QList<CompBase*> conlist;
    QList<QGraphicsItem*> itemlist = selectedItems();

    for( QGraphicsItem* item : itemlist )
    {
        if( item->type() == QGraphicsItem::UserType+1 ) // Component
        {
            Component* comp =  qgraphicsitem_cast<Component*>( item );
            if( comp && !comp->isHidden() && !comp->parentItem() ) complist.append( comp );
        }
        else if( item->type() == QGraphicsItem::UserType+2 ) // Wire
        {
            Wire* wire = qgraphicsitem_cast<Wire*>( item );
            if( !conlist.contains( wire ) ) conlist.append( wire );
    }   }

    QString circuit;
    for( CompBase* comp : complist ) circuit += comp->toString();
    for( CompBase* con  :  conlist ) circuit += con->toString();

    QApplication::clipboard()->setText( circuit );
}

void CanvasBase::paste( QPointF eventpoint )
{
    if( m_newWire ) return;
    stop();

    m_busy    = true;
    m_pasting = true;

    if( m_eventpoint == QPointF(1e6,1e6) ) // We don't have origin point
    {
        QRectF itemsRect = selectionArea().boundingRect();
        m_eventpoint = toGrid( itemsRect.topLeft() );
    }
    if( eventpoint == QPointF(1e6,1e6) ) eventpoint = m_eventpoint+QPointF( 8, 8 );

    for( QGraphicsItem*item : selectedItems() ) item->setSelected( false );

    m_deltaMove = toGrid(eventpoint) - m_eventpoint;

    QString circuit = QApplication::clipboard()->text();

    beginUndoStep();
    loadStrDoc( circuit );
    endUndoStep();

    m_pasting = false;
    m_busy = false;
}

void CanvasBase::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    if( event->button() == Qt::LeftButton )
    {
        if( m_newWire ) event->accept();
        QGraphicsScene::mousePressEvent( event );
    }
    else if( event->button() == Qt::RightButton )
    {
        if( m_newWire ) event->accept();
        else            QGraphicsScene::mousePressEvent( event );
    }
    else if( event->button() == Qt::MidButton )
        QGraphicsScene::mousePressEvent( event );
}

void CanvasBase::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
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

void CanvasBase::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
    if( m_newWire )
    {
        event->accept();
        if( event->modifiers() & Qt::ShiftModifier) m_newWire->m_freeLine = true;
        m_newWire->updateConRoute( event->scenePos() );
    }
    QGraphicsScene::mouseMoveEvent( event );
}


// UNDO REDO -----------------------------

void CanvasBase::undo()
{
    if( m_busy || m_deleting || m_newWire || m_undoIndex < 0 ) return; /// qDebug() << "\nCircuit::undo"<<m_undoIndex<<m_undoStack.size();
    m_undo = true;
    restoreState();
    m_undoIndex--;
    m_undo = false;
}

void CanvasBase::redo()
{
    if( m_busy || m_deleting || m_newWire || m_undoIndex >= (m_undoStack.size()-1) ) return;
    m_redo = true;
    m_undoIndex++;                                 /// qDebug() << "\nCircuit::redo"<<m_undoIndex<<m_undoStack.size();
    restoreState();
    m_redo = false;
}

void CanvasBase::setChanged()
{
    m_changed = true;
    QString title = MainWindow::self()->windowTitle();
    if( !title.endsWith('*') ) MainWindow::self()->setWindowTitle(title+'*');
}

void CanvasBase::saveChanges()
{
    if( m_newWire || m_circChange.size() == 0 ) return;
    setChanged();

    while( m_undoStack.size() > (m_undoIndex+1) ) m_undoStack.removeLast();

    m_undoStack.append( m_circChange );
    if( m_undoStack.size() > m_maxUndoSteps )
    {
        m_undoStack.takeFirst();
        m_undoIndex--;
    }
    m_undoIndex++;

    clearCircChanges();
    m_cicuitBatch = 0;  // Ends all CicuitChanges
    deleteRemoved();    // Delete Removed Components;

    /// qDebug() << "Circuit::saveChanges ---------------------------"<<m_undoIndex<<m_undoStack.size()<<endl;
}

void CanvasBase::saveCompChange( QString component, QString property, QString undoVal )
{
    clearCircChanges();
    addCompChange( component, property, undoVal );
    saveChanges();
}

void CanvasBase::addCompChange( QString component, QString property, QString undoVal )
{
    if( m_loading || m_deleting ) return;                      /// qDebug() << "Circuit::addCompChange      " << component << property;// << value;
    compChange cChange{ component, property, undoVal, "" };
    m_circChange.compChanges.append( cChange );
}

void CanvasBase::removeLastUndo()
{
    if( m_undoStack.isEmpty() ) return;
    m_undoStack.takeLast();
    m_undoIndex--;
}


void CanvasBase::deleteRemoved()
{
    for( CompBase* comp : m_removedComps ) delete comp;
    m_removedComps.clear();
}

void CanvasBase::beginCircuitBatch() // Don't create/remove
{
    /// qDebug() << "Circuit::beginCircuitBatch";
    if( !m_cicuitBatch ) clearCircChanges();
    m_cicuitBatch++;
}

void CanvasBase::endCircuitBatch() // Don't create/remove
{
    /// qDebug() << "Circuit::endCircuitBatch";
    if( m_cicuitBatch > 0 ){
        m_cicuitBatch--;
        if( m_cicuitBatch == 0 ) saveChanges();
    }
}


void CanvasBase::endUndoStep()   //
{
    calculateChanges();
    endCircuitBatch();
}

void CanvasBase::cancelUndoStep()
{
    calculateChanges();
    if( m_circChange.size() )
    {
        endCircuitBatch();
        undo();
        m_undoStack.takeLast();
    }
    else m_cicuitBatch = 0;
    /// qDebug() << "Circuit::cancelUndoStep--------------------------------"<<endl;
}

#include "moc_canvasbase.cpp"
