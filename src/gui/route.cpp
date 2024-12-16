/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "route.h"
#include "pinbase.h"

#include "stringprop.h"

Route::Route( QString id, PinBase* startpin )
     : CompBase( id )
{
    m_startPin  = startpin;
    m_actLine   = 0;
    m_lastIndex = 0;

    addPropGroup( {"Hidden", {
        new StrProp<Route>( "pin0" ,"","", this, &Route::startPinId, &Route::dummySetter ),
        new StrProp<Route>( "pin1" ,"","", this, &Route::endPinId,   &Route::dummySetter ),
        new StrProp<Route>( "pList","","", this, &Route::pListStr,   &Route::setPointListStr )
    }, 0} );
}
Route::~Route(){}

QString Route::startPinId() { return m_startPin->pinId(); }
QString Route::endPinId()   { return m_endPin->pinId(); }

void Route::writeWireFlag( int flag, bool val )
{
    if( val ) m_wireFlags |= flag;
    else      m_wireFlags &= !(~flag);
}
