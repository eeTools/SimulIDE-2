/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QDebug>

#include <QPainter>
#include <QCursor>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsProxyWidget>

#include "fblock.h"
#include "hook.h"
#include "composer.h"
#include "module.h"
#include "blocklist.h"
#include "composerview.h"
#include "composerwidget.h"
#include "propdialog.h"
#include "utils.h"

#include "m_property.h"
#include "comproperty.h"
#include "propwidget.h"
#include "modsignal.h"

#define HOOK_HEIGHT 8
#define HEAD_HEIGHT 16

FuncBlock::FuncBlock( fComponent* fComp, QString type, int id )
         : QGraphicsItem()
{
    m_id = id;
    //m_fComp  = fComp;
    m_propDialog = nullptr;

    m_type = type;
    m_module = (Module*)BlockList::self()->createItem( type, m_id );
    if( !m_module ) m_module = new Module( id );
    m_module->setComponent( fComp );
    m_module->setFuncBlock( this );

    m_color  = QColor( 85, 85, 90 );
    m_hColor = QColor( 240, 240, 215 );
    m_pColor = QColor( 105, 105, 95 );

    m_font.setFamily("Ubuntu Mono");
    m_font.setBold( true );
    m_font.setPixelSize( 11 );
    m_font.setStretch( 100 );
    m_font.setLetterSpacing( QFont::PercentageSpacing, 100 );
#ifndef Q_OS_UNIX
    font.setLetterSpacing( QFont::PercentageSpacing, 87 );
    //font.setStretch( 99 );
#endif

    setCursor( Qt::OpenHandCursor );
    setFlag( QGraphicsItem::ItemIsSelectable, true );

    setup();
}
FuncBlock::~FuncBlock(){}

void FuncBlock::setPropStr( QString name, QString value )
{
    if     ( name == "x" ) setX( value.toDouble() );
    else if( name == "y" ) setY( value.toDouble() );
    else if( name == "props" )
    {
        QStringList propList = value.split(",");
        for( QString propId : propList )
        {
            if( propId.isEmpty() ) continue;
            Hook* hook = m_propSlotMap.value( propId );
            if( !hook ) continue;
            hook->setVisible( true );

            hook = m_propSignalMap.value( propId );
            if( hook ) hook->setVisible( true );
        }
        updateSize();
    }
    else m_module->setPropStr( name, value );
}

void FuncBlock::remove()
{
    for( Hook* hook : m_propSignals ) hook->removeWire();
    for( Hook* hook : m_propSlots   ) hook->removeWire();
    for( Hook* hook : m_signalHooks ) hook->removeWire();
    for( Hook* hook : m_slotHooks   ) hook->removeWire();
    delete m_module;
}

void FuncBlock::setup()
{
    QList<propGroup> groups = m_module->getPropGroups();
    for( propGroup group : groups )
    {
        if( group.flags & groupHidden ) continue;

        QList<ComProperty*> propList = group.propList;
        if( propList.isEmpty() ) continue;

        for( ComProperty* prop : propList )            // Add Properties
        {
            QString propId = prop->idStr();
            //if( prop->flags() & propHidden ) continue; // Property hidden
            if( propId == "" )           continue; // Just a label

            bool visible = false;
            QString hookLabel = propId;
            if( propId == "propval") // This is a Property Module
            {
                mProperty* pm = (mProperty*)m_module;
                hookLabel = pm->propName();
                visible = true;
            }
            // We use propWidget in properties dialog to call us
            // Then we show/hide the hook here
            QObject::connect( prop->getWidget(), &PropWidget::showToggled,
                              [=](QString propId, bool checked){ showToggled( propId, checked ); } );

            Hook* hook0 = new Hook( 180, QPoint(-60, 0 ), propId+"Slot@"+m_id, hookProperty, this );
            hook0->setLabelText( hookLabel );
            hook0->setFontSize( 9 );
            hook0->setVisible( visible );
            m_propSlots.append( hook0 );
            m_propSlotMap.insert( hookLabel, hook0 );

            ///if( !(prop->flags() & propSignal) ) continue;
            Hook* hook1 = new Hook( 0, QPoint( 60, 0 ), propId+"Signal@"+m_id, hookProperty, this );
            m_propSignals.append( hook1 );
            m_propSignalMap.insert( hookLabel, hook1 );
        }
    }
    updateWidget();
}

void FuncBlock::showToggled( QString propId, bool checked )
{
    Hook* hook = m_propSlotMap.value( propId );
    if( !hook ) return;

    hook->setVisible( checked );
    if( !checked ) hook->removeWire();

    hook = m_propSignalMap.value( propId );
    if( hook )
    {
        hook->setVisible( checked );
        if( !checked ) hook->removeWire();
    }
    updateSize();
}

void FuncBlock::updateWidget()
{
    updateSlots();
    updateSignals();
    updateSize();
}

void FuncBlock::updateSize()
{
    int  sizeIn  = m_slotHooks.size();
    int  sizeOut = m_signalHooks.size();

    bool fullSize = sizeIn && sizeOut;
    int width     = fullSize ? 80 : 60;

    int propSlots = 0;
    for( Hook* hook : m_propSlots )
    {
        if( !hook->isVisible() ) continue;
        hook->setY( (propSlots+1)*HOOK_HEIGHT );

        QString propId = m_propSlotMap.key( hook );
        propId.replace("Slot", "Signal");
        hook = m_propSignalMap.value( propId );
        if( hook ) {
            hook->setX( 60-width );
            hook->setY( (propSlots+1)*HOOK_HEIGHT );
        }
        propSlots++;
    }
    if( propSlots > 0 ) propSlots++;

    int hookSize = (sizeIn > sizeOut) ? sizeIn : sizeOut;
    m_area     = QRectF(-60,-HEAD_HEIGHT, width, HEAD_HEIGHT+(propSlots+hookSize+1)*HOOK_HEIGHT );
    m_bodyArea = QRectF(-60,-HEAD_HEIGHT, width, HEAD_HEIGHT+(propSlots+hookSize+1)*HOOK_HEIGHT );
    m_headArea = QRectF(-60,-HEAD_HEIGHT, width, HEAD_HEIGHT );
    m_PropArea = QRectF(-60,-HEAD_HEIGHT, width, HEAD_HEIGHT+propSlots*HOOK_HEIGHT );

    for( int i=0; i<m_slotHooks.size();   ++i ) m_slotHooks.at( i )->setY( (propSlots+i+1)*HOOK_HEIGHT );
    for( int i=0; i<m_signalHooks.size(); ++i ) m_signalHooks.at( i )->setY( (propSlots+i+1)*HOOK_HEIGHT );

    Composer::self()->update();
}

void FuncBlock::updateSignals()
{
    bool fullSize = m_slotHooks.size() > 0;
    int posX = fullSize ? 20 : 0;

    std::vector<ModSignal*> signList = m_module->getSignals();
    int newSize = signList.size();
    int oldSize = m_signalHooks.size();

    if( newSize > oldSize )    // Create Signal Hooks
    {
        for( int i=oldSize; i<newSize; ++i )
        {
            ModSignal* signal = signList.at(i);
            QString name = signal->name();
            Hook*   hook = new Hook( 0, QPoint( posX, 0 ), name+"Signal@"+m_id, signal->type(), this );
            hook->setLabelText( name );
            hook->setFontSize( 9 );
            m_signalHooks.append( hook );
        }
    }else                     // Delete Signal Hooks
    {
        for( int i=newSize; i<oldSize; ++i )
        {
            Hook* hook = m_signalHooks.takeLast();
            delete hook;
        }
    }
}

void FuncBlock::updateSlots()
{
    std::vector<ModSlot*> slotList = m_module->getSlots();
    int newSize = slotList.size();
    int oldSize = m_slotHooks.size();

    if( newSize > oldSize )    // Create Slot Hooks
    {
        for( int i=oldSize; i<newSize; ++i )
        {
            ModSlot* slot = slotList.at(i);
            QString  name = slot->name();

            Hook* hook = new Hook( 180, QPoint(-60, 0 ), name+"Slot@"+m_id, slot->type(), this );
            hook->setLabelText( name );
            hook->setFontSize( 9 );
            m_slotHooks.append( hook );
        }
    }else                     // Delete Slot Hooks
    {
        for( int i=newSize; i<oldSize; ++i )
        {
            Hook* hook = m_slotHooks.takeLast();
            delete hook;
        }
    }
}

void FuncBlock::renamePropHooks( QString oldName, QString newName )
{
    Hook* hook = m_propSlotMap.value( oldName );
    if( !hook ) return;

    m_propSlotMap.remove( oldName );
    m_propSlotMap.insert( newName, hook );
    hook->setLabelText( newName );

    hook = m_propSignalMap.value( oldName );
    hook->setPinId( newName.toLower()+"Signal@"+m_id );
    m_propSignalMap.remove( oldName );
    m_propSignalMap.insert( newName, hook );
}

QString FuncBlock::toString()
{
    QString fblock = m_module->toString();
    fblock.remove("\n");
    fblock += "; x="+QString::number( this->pos().x() );
    fblock += "; y="+QString::number( this->pos().y() );

    QString propHooks;
    for( QString hookId : m_propSlotMap.keys() )
    {
        Hook* hook = m_propSlotMap.value( hookId );
        if( !hook->isVisible() ) continue;
        propHooks += hookId+",";
    }
    if( !propHooks.isEmpty() ) fblock += "; props="+propHooks;

    return "\n"+fblock+"\n";
}

void FuncBlock::move( QPointF delta )
{
    setPos( toGrid( pos() + delta ) );
    for( Hook* hook : m_propSlots   ) hook->isMoved();
    for( Hook* hook : m_propSignals ) hook->isMoved();
    for( Hook* hook : m_signalHooks ) hook->isMoved();
    for( Hook* hook : m_slotHooks   ) hook->isMoved();
}

void FuncBlock::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    if( event->button() == Qt::LeftButton )
    {
        event->accept();

        if( event->modifiers() & Qt::ControlModifier ) setSelected( !isSelected() );
        else{
            QList<QGraphicsItem*> itemlist = Composer::self()->selectedItems();
            if( !isSelected() )     // Unselect everything and select this
            {
                for( QGraphicsItem* item : itemlist ) item->setSelected( false );
                setSelected( true );
            }else{                                     // Deselect childs
                for( QGraphicsItem* item : itemlist )
                {
                    QList<QGraphicsItem*> childs = item->childItems();
                    for( QGraphicsItem* child : childs ) child->setSelected( false );
            }   }
            setCursor( Qt::ClosedHandCursor );
        }
        QApplication::focusWidget()->clearFocus();
        ComposerView::self()->setFocus();
}   }

void FuncBlock::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
    event->accept();

    QPointF delta = toGrid(event->scenePos()) - toGrid(event->lastScenePos());

    QList<QGraphicsItem*> itemlist = Composer::self()->selectedItems();

    if( !m_moving )         // Get lists of elements to move and save Undo state
    {
        /// TODO: for Undo/Redo FuncBlock should be CompBase ???
        //Composer::self()->beginCircuitBatch();

        m_blockMoveList.clear();

        for( QGraphicsItem* item : itemlist )
        {
            if( item->type() == UserType+10 )     // Functional Block selected
            {
                FuncBlock* block =  qgraphicsitem_cast<FuncBlock*>( item );
                //Composer::self()->addCompChange( block->getUid(), "Pos", block->getPropStr("Pos") );
                m_blockMoveList.append( block );
        }   }

        m_moving = true;
        //Composer::self()->saveChanges();
    }

    for( FuncBlock* block : m_blockMoveList ) block->move( delta ); // Move Components selected

    Composer::self()->update();
}

void FuncBlock::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
    event->accept();
    setCursor( Qt::OpenHandCursor );

    if( m_moving ) m_moving = false;

    Composer::self()->update();
}

void FuncBlock::mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event )
{
    if( event->button() == Qt::LeftButton ) slotProperties();
}

void FuncBlock::slotProperties()
{
    if( !m_propDialog )
    {
        m_propDialog = new PropDialog( ComposerWidget::self(), "" );
        m_propDialog->setComponent( m_module, true );
    }
    m_propDialog->show();
}

void FuncBlock::paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* )
{
    QPen pen( Qt::black, 1.3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen );

    p->setBrush( m_color );
    p->drawRoundedRect( m_bodyArea, 5, 5 );

    pen.setWidth( 1 );
    p->setPen( pen );

    if( m_PropArea.height() > HEAD_HEIGHT ) {
        p->setBrush( m_pColor );
        p->drawRoundedRect( m_PropArea, 5, 5 );
    }
    p->setBrush( m_hColor );
    p->drawRoundedRect( m_headArea, 5, 5 );

    pen.setBrush( QColor( 20, 50, 120 ) );
    p->setPen( pen );
    p->setFont( m_font );
    p->drawText( m_headArea, Qt::AlignCenter, m_type );

    if( this->isSelected() )
    {
        p->setOpacity( 0.3 );
        p->setBrush( QColor( 255, 255, 0 ) );
        p->drawRoundedRect( m_bodyArea, 5, 5 );
    }
}
