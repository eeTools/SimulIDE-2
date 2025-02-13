/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "ampmeter.h"

#define tr(str) simulideTr("Ampmeter",str)

listItem_t AmpMeter::registerItem(){
    return {
        tr("Ampmeter"),
        "Meters",
        "ampmeter.png",
        "AmpMeter",
        [](int id){ return (CompBase*)new AmpMeter( id ); } };
}

AmpMeter::AmpMeter( int id )
        : Meter( id )
{
    m_unit = "A";
    m_dispValue = 0;
    setResistance( 1e-6 );
    m_display.setText(" 0.000\n A");
}
AmpMeter::~AmpMeter(){}

void AmpMeter::updateStep()
{
    /*double curr = current();
    
    if( curr != m_dispValue )
    {
        m_dispValue = curr;
        Meter::updateStep();
    }*/
}
