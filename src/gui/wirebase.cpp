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
        {"pin0" ,"","", nullptr, P_String, P_NoSet },
        {"pin1" ,"","", nullptr, P_String, P_NoSet },
        {"pList","","", nullptr, P_String, P_NoSet }
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

QString WireBase::toString() // Used to save circuit
{
    QString item = "\n"+m_type;
    item += "; pin0="+startPinId();
    item += "; pin1="+endPinId();
    item += "; pList="+pListStr();
    item += "\n";

    return item;
}
