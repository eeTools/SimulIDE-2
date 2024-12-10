/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "ampmeter.h"

#define tr(str) simulideTr("Amperimeter",str)

listItem_t Amperimeter::libraryItem(){
    return {
        tr("Ampmeter"),
        "Meters",
        "amperimeter.png",
        "Amperimeter",
        [](QString type, QString id){ return (Component*)new Amperimeter( type, id ); } };
}

Amperimeter::Amperimeter( QString type, QString id )
           : Meter( type, id )
{
    m_unit = "A";
    m_dispValue = 0;
    setResistance( 1e-6 );
    m_display.setText(" 0.000\n A");
}
Amperimeter::~Amperimeter(){}

void Amperimeter::updateStep()
{
    /*double curr = current();
    
    if( curr != m_dispValue )
    {
        m_dispValue = curr;
        Meter::updateStep();
    }*/
}
