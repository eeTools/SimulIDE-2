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

#include "comproperty.h"
#include "propwidget.h"
#include "modsignal.h"

#define HOOK_HEIGHT 8
#define HEAD_HEIGHT 16

FuncBlock::FuncBlock( fComponent* fComp, Module* module, QString type, QString id )
         : QGraphicsItem()
{
    m_id = id;
    m_fComp  = fComp;
    m_module = module;
    m_propDialog = nullptr;

    if( module )      // Created from Component file
    {
        m_type = module->itemType();
    }
    else              // Created from Composer list
    {
        m_type = type;
        m_module = (Module*)BlockList::self()->createItem( type, m_id );
    }
    m_color  = QColor( 85, 85, 90 );
    m_hColor = QColor( 240, 240, 215 );
    m_pColor = QColor( 105, 105, 95 );

    m_font.setFamily("Ubuntu Mono");
    m_font.setPixelSize( 13 );
    m_font.setStretch( 100 );
    m_font.setLetterSpacing( QFont::PercentageSpacing, 100 );
#ifndef Q_OS_UNIX
    font.setLetterSpacing( QFont::PercentageSpacing, 87 );
    //font.setStretch( 99 );
#endif

    setCursor( Qt::OpenHandCursor );
    setFlag( QGraphicsItem::ItemIsSelectable, true );

    if( m_module ) setup();
}
FuncBlock::~FuncBlock(){}

void FuncBlock::remove()
{
    for( Hook* hook : m_signalHooks ) hook->removeWire();
    for( Hook* hook : m_slotHooks   ) hook->removeWire();
    delete m_module;
}

void FuncBlock::setup()
{
    //qDebug() << "FuncBlock::setup"<< m_fComp;
    m_module->setFuncBlock( this );
    m_module->setComponent( m_fComp );

    //m_propSize= 0;
    int p = 0;
    QList<propGroup>* groups = m_module->properties();
    for( propGroup group : *groups )
    {
        if( group.flags & groupHidden ) continue;

        QList<ComProperty*> propList = group.propList;
        if( propList.isEmpty() ) continue;

        for( ComProperty* prop : propList )            // Add Properties
        {
            //if( prop->flags() & propHidden ) continue; // Property hidden
            if( prop->id() == "" )           continue; // Just a label

            // We use propWidget in properties dialog to call us
            // Then we show/hide the hook here
            QObject::connect( prop->getWidget(), &PropWidget::showToggled,
                              [=](QString propId, bool checked){ showToggled( propId, checked ); } );

            QString propId = prop->id();

            //if( prop->flags() & propSlot )
            {
                Hook* hook0 = new Hook( 180, QPoint(-60, p*HOOK_HEIGHT ), propId+"@"+m_id,-1, hookProperty, this );
                hook0->setLabelText( prop->label() );
                hook0->setFontSize( 10 );
                hook0->setVisible( false );
                m_propHooks.insert( propId, hook0 );
            }
            /*if( prop->flags() & propSignal ){
                Hook* hook1 = new Hook( 0, QPoint( 60, p*HOOK_HEIGHT ), propId+"@"+m_id,-1, hookProperty, this );
                //hook1->setLabelText( prop->label() );
                m_signalHooks.append( hook1 );
                m_propSignals++;
            }*/
            p++;
        }
    }

    updateWidget();
}

void FuncBlock::showToggled( QString propId, bool checked )
{
    Hook* hook = m_propHooks.value( propId );
    if( !hook ) return;

    hook->setVisible( checked );
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
    int propSlots = visibleProperties();
    if( propSlots > 0 ) propSlots++;
    int  sizeIn   = m_slotHooks.size();
    int  sizeOut  = m_signalHooks.size();

    bool fullSize = sizeIn && sizeOut;
    int width     = fullSize ? 120 : 60;

    int hookSize = (sizeIn > sizeOut) ? sizeIn : sizeOut;
    m_area     = QRectF(-60  ,  -HEAD_HEIGHT    , width  , HEAD_HEIGHT+(propSlots+hookSize+1)*HOOK_HEIGHT+2 );
    m_bodyArea = QRectF(-60  , 2-HEAD_HEIGHT    , width  , HEAD_HEIGHT+(propSlots+hookSize+1)*HOOK_HEIGHT );
    m_headArea = QRectF(-59.5, 2-HEAD_HEIGHT-0.5, width-1, HEAD_HEIGHT );
    m_PropArea = QRectF(-59.5, 2-HEAD_HEIGHT-0.5, width-1, HEAD_HEIGHT+propSlots*HOOK_HEIGHT );

    for( int i=0; i<m_slotHooks.size();   ++i ) m_slotHooks.at( i )->setY( (propSlots+i+1)*HOOK_HEIGHT );
    for( int i=0; i<m_signalHooks.size(); ++i ) m_signalHooks.at( i )->setY( (propSlots+i+1)*HOOK_HEIGHT );

    Composer::self()->update();
}

void FuncBlock::updateSignals()
{
    bool fullSize = m_slotHooks.size() > 0;
    int posX = fullSize ? 60 : 0;

    std::vector<ModSignal*> signList = m_module->getSignals();
    int newSize = signList.size();
    int oldSize = m_signalHooks.size();

    if( newSize > oldSize )    // Create Signal Hooks
    {
        for( int i=oldSize; i<newSize; ++i )
        {
            ModSignal* signal = signList.at(i);
            QString name = signal->name();
            Hook*   hook = new Hook( 0, QPoint( posX, 0 ), name+"@"+m_id, i, signal->type(), this );
            hook->setLabelText( name );
            hook->setFontSize( 10 );
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

            Hook* hook = new Hook( 180, QPoint(-60, 0 ), name+"@"+m_id,-1, slot->type(), this );
            hook->setLabelText( name );
            hook->setFontSize( 10 );
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

Hook* FuncBlock::getHook( QString id )
{
    Hook* hook = getSignalHook( id );
    if( !hook ) hook = getSlotHook( id );
    return hook;
}

Hook* FuncBlock::getSignalHook( QString id )
{
    for( Hook* hook : m_signalHooks ) if( hook->pinId() == id ) return hook;
    return nullptr;
}

Hook* FuncBlock::getSlotHook( QString id )
{
    for( Hook* hook : m_slotHooks ) if( hook->pinId() == id ) return hook;
    return nullptr;
}

QString FuncBlock::toString()
{
    QString fblock = m_module->toString();
    fblock += "Fblock";
    fblock += "; uid="+m_id;
    fblock += "; x="+QString::number( this->pos().x() );
    fblock += "; y="+QString::number( this->pos().y() );
    fblock += "\n";
    return fblock;
}

void FuncBlock::move( QPointF delta )
{
    setPos( pos() + delta );
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

int FuncBlock::visibleProperties()
{
    int propSlots = 0;
    for( Hook* hook : m_propHooks ) if( hook->isVisible() ) propSlots++;
    return propSlots;
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

    p->setBrush( QColor( 0, 0, 20 ) );
    p->setFont( m_font );
    p->drawText( m_headArea, Qt::AlignCenter, m_type );
}
