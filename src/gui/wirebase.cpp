/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "wirebase.h"
#include "pinbase.h"

#include "stringprop.h"

WireBase::WireBase( QString id, PinBase* startpin )
        : CompBase( id )
{
    m_startPin  = startpin;
    m_actLine   = 0;
    m_lastIndex = 0;

    m_wireFlags = 0;

    addPropGroup( {"Hidden", {
        new StrProp<WireBase>( "pin0" ,"","", this, &WireBase::startPinId, &WireBase::dummySetter ),
        new StrProp<WireBase>( "pin1" ,"","", this, &WireBase::endPinId,   &WireBase::dummySetter ),
        new StrProp<WireBase>( "pList","","", this, &WireBase::pListStr,   &WireBase::setPointListStr )
    }, 0} );
}
WireBase::~WireBase(){}

QString WireBase::startPinId() { return m_startPin->pinId(); }
QString WireBase::endPinId()   { return m_endPin->pinId(); }

void WireBase::writeWireFlag( int flag, bool val )
{
    if( val ) m_wireFlags |= flag;
    else      m_wireFlags &= !(~flag);
}
