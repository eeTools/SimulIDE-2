/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "voltmeter.h"
#include "pin.h"

#define tr(str) simulideTr("Voltmeter",str)

listItem_t Voltmeter::libraryItem(){
    return {
        tr("Voltmeter"),
        "Meters",
        "voltmeter.png",
        "Voltmeter",
        [](QString id){ return (Component*)new Voltmeter( id ); } };
}

Voltmeter::Voltmeter( QString id )
          : Meter( id )
{
    m_unit = "V";
    setResistance( high_imp );
    m_display.setText( " 0.000\n V");
}
Voltmeter::~Voltmeter(){}

void Voltmeter::updateStep()
{
    double volt = m_pin[0]->getVoltage()-m_pin[1]->getVoltage();
    
    if( volt != m_dispValue )
    {
        m_dispValue = volt;
        Meter::updateStep();
}   }
