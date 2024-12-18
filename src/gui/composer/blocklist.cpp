/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QMimeData>
#include <QDrag>

#include "blocklist.h"
#include "mainwindow.h"

BlockList* BlockList::m_pSelf = nullptr;

BlockList::BlockList( QWidget* parent )
         : ListBase( parent )
{
    m_pSelf = this;

    m_customComp = false;
    registerItems();
    m_customComp = true;
}
BlockList::~BlockList(){}

// --------------------------------------------
// BEGIN Item includes

#include "m_bitop.h"
#include "m_abop.h"
#include "m_bittoint.h"
#include "m_inttobit.h"
#include "m_ioport.h"
#include "m_iohook.h"
#include "m_delay.h"
#include "m_wavegen.h"
#include "m_dial.h"
#include "m_property.h"

void BlockList::registerItems()
{
    addCategory( tr("Ports"),"Ports","", "" );
    addItem( mIoPort::registerItem() );
    addItem( IoHook::registerItem() );
    addCategory( tr("Operations"),"Operations","","" );
    addItem( BitOp::registerItem() );
    addItem( AbOp::registerItem() );
    addCategory( tr("Converters"),"Converters","","" );
    addItem( BitToInt::registerItem() );
    addItem( IntToBit::registerItem() );
    addCategory( tr("Controls"),"Controls","","" );
    addItem( mDial::registerItem() );
    addCategory( tr("Other"),"Other","","" );
    addItem( WaveGen::registerItem() );
    addItem( Delay::registerItem() );
    addItem( PropertyM::registerItem() );
}
