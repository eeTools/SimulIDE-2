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
        m_module = BlockList::self()->createModule( type, m_id );
    }
    m_color  = QColor( 105, 105, 100 );
    m_hColor = QColor( 240, 240, 215 );

    m_font.setFamily("Ubuntu Mono");
    m_font.setPixelSize( 14 );
    m_font.setStretch( 100 );
    m_font.setLetterSpacing( QFont::PercentageSpacing, 100 );
#ifndef Q_OS_UNIX
    font.setLetterSpacing( QFont::PercentageSpacing, 87 );
    //font.setStretch( 99 );
#endif

    setCursor( Qt::OpenHandCursor );
    setFlag( QGraphicsItem::ItemIsSelectable, true );

    m_propSlots   = 0;
    m_propSignals = 0;

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
    //m_module->setup();

    m_propSize= 0;
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

            //QString type = prop->type();
            if( (prop->flags() & propSignal) || (prop->flags() & propSlot ) )
            {
                PropWidget* mp = prop->getWidget();        // Property widget
                if( !mp ) continue;

                mp->setup( false );
                mp->setFixedSize( 100, 18 );
                mp->setStyleSheet("font-size:11px; font-family:Ubuntu Mono; background-color:#696964; color:#EBFFFF;");

                QGraphicsProxyWidget* proxy = Composer::self()->addWidget( mp );
                proxy->setZValue( 100-p );
                proxy->setParentItem( this );
                proxy->setPos( QPoint(-50, p*20-8) );

                if( prop->flags() & propSlot ){
                    Hook* hook0 = new Hook( 180, QPoint(-60, p*20 ), prop->id()+"@"+m_id,-1, hookProperty, this );
                    m_slotHooks.append( hook0);
                    m_propSlots++;
                }
                if( prop->flags() & propSignal ){
                    Hook* hook1 = new Hook( 0, QPoint( 60, p*20 ), prop->id()+"@"+m_id,-1, hookProperty, this );
                    m_signalHooks.append( hook1 );
                    m_propSignals++;
                }
                p++;
            }
        }
        m_propSize += p;
    }

    updateSlots();
    updateSignals();

    /*if( m_type == "IoPin")
    {
        ComProperty* p = m_module->getProperty("Output");
        if( p ){
            /// ModSlot* cb = new SetIntSlot<FuncBlock>( "name", this, &FuncBlock::ioPinDir );
            /// p->addCallBack( cb );
            ioPinDir( p->getValStr() == "1" );
        }
    }*/
    updateSize();

    /// TODO: Add secondary property groups as dropdowns
}

void FuncBlock::updateSignals()
{
    bool fullSize = m_propSize > 0 || m_slotHooks.size() > 0;
    int posX = fullSize ? 60 : 0;

    std::vector<ModSignal*> signList = m_module->getSignals();
    int newSize = signList.size();
    int oldSize = m_signalHooks.size()-m_propSignals;

    if( newSize > oldSize )    // Create Signal Hooks
    {
        for( int i=oldSize; i<newSize; ++i )
        {
            ModSignal* signal = signList.at(i);
            QString name = signal->name();
            Hook*   hook = new Hook( 0, QPoint( posX, m_propSize*20+i*10 ), name+"@"+m_id, i, signal->type(), this );
            hook->setLabelText( name );
            m_signalHooks.append( hook );
        }
    }else                     // Delete Signal Hooks
    {
        for( int i=newSize+m_propSignals; i<oldSize+m_propSignals; ++i )
        {
            Hook* hook = m_signalHooks.at(i);
            m_signalHooks.removeOne( hook );
            delete hook;
        }
    }
}

void FuncBlock::updateSlots()
{
    std::vector<ModSlot*> slotList = m_module->getSlots();
    int newSize = slotList.size();
    int oldSize = m_slotHooks.size()-m_propSlots;

    if( newSize > oldSize )    // Create Slot Hooks
    {
        for( int i=oldSize; i<newSize; ++i )
        {
            ModSlot* slot = slotList.at(i);
            QString  name = slot->name();

            Hook* hook = new Hook( 180, QPoint(-60, m_propSize*20+i*10 ), name+"@"+m_id,-1, slot->type(), this );
            hook->setLabelText( name );
            m_slotHooks.append( hook );
        }
    }else                     // Delete Slot Hooks
    {
        for( int i=newSize+m_propSlots; i<oldSize+m_propSlots; ++i )
        {
            Hook* hook = m_slotHooks.at(i);
            m_slotHooks.removeOne( hook );
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

void FuncBlock::updateSize()
{
    int  sizeIn   = (m_slotHooks.size()-m_propSlots)*10;
    int  sizeOut  = (m_signalHooks.size()-m_propSignals)*10;
    bool fullSize = m_propSize > 0 || (sizeIn && sizeOut);
    int width     = fullSize ? 120 : 60;

    int hookSize = (sizeIn > sizeOut) ? sizeIn : sizeOut;
    m_area   = QRectF(-60,-30, width, 30+m_propSize*20+hookSize );
    m_header = QRectF(-59.5,-29.5, width-1, 18 );

    Composer::self()->update();
}

/*void FuncBlock::ioPinDir( int isOut )
{
    Hook* inHook = getHook("input@"+m_id );
    if( inHook ) inHook->setVisible( isOut==1 );
    Hook* outHook = getHook("output@"+m_id );
    if( outHook ) outHook->setVisible( isOut==0 );
}*/

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

    QPointF delta = event->scenePos() - event->lastScenePos();

    QList<QGraphicsItem*> itemlist = Composer::self()->selectedItems();

    if( !m_moving )         // Get lists of elements to move and save Undo state
    {
        //Composer::self()->beginComposerBatch();

        m_blockMoveList.clear();

        for( QGraphicsItem* item : itemlist )
        {
            if( item->type() == UserType+10 )     // Component selected
            {
                FuncBlock* block =  qgraphicsitem_cast<FuncBlock*>( item );
                //Composer::self()->addCompChange( comp->getUid(), "Pos", comp->getPropStr("Pos") );
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

    //p->fillRect( m_area, color );
    p->drawRoundedRect( m_area, 5, 5 );

    pen.setWidth( 1 );
    p->setPen( pen );
    p->setBrush( m_hColor );
    p->drawRoundedRect( m_header, 5, 5 );

    p->setBrush( QColor( 0, 0, 20 ) );
    p->setFont( m_font );
    p->drawText( m_header, Qt::AlignCenter, m_type );
}
