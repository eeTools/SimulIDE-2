/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "voltmeter.h"
#include "pin.h"

#define tr(str) simulideTr("Voltimeter",str)

listItem_t Voltimeter::libraryItem(){
    return {
        tr("Voltmeter"),
        "Meters",
        "voltmeter.png",
        "Voltmeter",
        [](QString type, QString id){ return (Component*)new Voltimeter( type, id ); } };
}

Voltimeter::Voltimeter( QString type, QString id )
          : Meter( type, id )
{
    m_unit = "V";
    setResistance( high_imp );
    m_display.setText( " 0.000\n V");
}
Voltimeter::~Voltimeter(){}

void Voltimeter::updateStep()
{
    double volt = m_pin[0]->getVoltage()-m_pin[1]->getVoltage();
    
    if( volt != m_dispValue )
    {
        m_dispValue = volt;
        Meter::updateStep();
}   }
