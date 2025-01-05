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

#include "m_unaryop.h"
#include "m_binaryop.h"
#include "m_bittoint.h"
#include "m_inttobit.h"
#include "m_floatop.h"
#include "m_pinport.h"
#include "m_hookport.h"
#include "m_delay.h"
#include "m_wavegen.h"
#include "m_dial.h"
#include "m_button.h"
#include "m_property.h"
#include "m_config.h"

void BlockList::registerItems()
{
    addCategory( tr("Ports"),"Ports","", "" );
    addItem( mPinPort::registerItem() );
    addItem( mHookPort::registerItem() );
    addCategory( tr("Operations"),"Operations","","" );
    addItem( UnaryOp::registerItem() );
    addItem( BinaryOp::registerItem() );
    addItem( FloatOp::registerItem() );
    addCategory( tr("Converters"),"Converters","","" );
    addItem( BitToInt::registerItem() );
    addItem( IntToBit::registerItem() );
    addCategory( tr("Controls"),"Controls","","" );
    addItem( mButton::registerItem() );
    addItem( mDial::registerItem() );
    addCategory( tr("Other"),"Other","","" );
    addItem( WaveGen::registerItem() );
    addItem( Delay::registerItem() );
    addItem( mProperty::registerItem() );
    addItem( mConfig::registerItem() );
}
