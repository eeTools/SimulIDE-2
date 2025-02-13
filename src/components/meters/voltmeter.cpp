/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "voltmeter.h"
#include "pin.h"

#define tr(str) simulideTr("VoltMeter",str)

listItem_t VoltMeter::registerItem(){
    return {
        tr("Voltmeter"),
        "Meters",
        "voltmeter.png",
        "VoltMeter",
        [](int id){ return (CompBase*)new VoltMeter( id ); } };
}

VoltMeter::VoltMeter( int id )
         : Meter( id )
{
    m_unit = "V";
    setResistance( high_imp );
    m_display.setText( " 0.000\n V");
}
VoltMeter::~VoltMeter(){}

void VoltMeter::updateStep()
{
    double volt = m_pin[0]->getVoltage()-m_pin[1]->getVoltage();
    
    if( volt != m_dispValue )
    {
        m_dispValue = volt;
        Meter::updateStep();
}   }
