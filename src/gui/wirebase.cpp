/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "wirebase.h"
#include "pinbase.h"

WireBase::WireBase( int id, PinBase* startpin )
        : CompBase( id )
{
    m_startPin  = startpin;
    m_actLine   = 0;
    m_lastIndex = 0;

    m_wireFlags = 0;

    /*addPropGroup( {"Hidden", {}, 0 },
    {
        {"pin0" ,"","", P_String, 0, nullptr },
        {"pin1" ,"","", P_String, 0, nullptr },
        {"pList","","", P_String, 0, nullptr }
    });*/
}
WireBase::~WireBase(){}

QString WireBase::startPinId() { return m_startPin->pinId(); }
QString WireBase::endPinId()   { return m_endPin->pinId(); }

void WireBase::writeWireFlag( int flag, bool val )
{
    if( val ) m_wireFlags |= flag;
    else      m_wireFlags &= !(~flag);
}
