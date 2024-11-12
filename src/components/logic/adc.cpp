/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>

#include "adc.h"
#include "simulator.h"
#include "iopin.h"

#include "doubleprop.h"
#include "intprop.h"

#define tr(str) simulideTr("ADC",str)

listItem_t ADC::libraryItem(){
    return {
        tr("ADC"),
        "Other Logic",
        "1to3.png",
        "ADC",
        [](QString type, QString id){ return (Component*)new ADC( type, id ); } };
}

ADC::ADC( QString type, QString id )
   : LogicComponent( type, id )
{
    m_width  = 4;
    m_height = 9;

    setLabelPos(-16,-80, 0);
    setNumOutputs( 8 );    // Create Output Pins
    setNumInps( 1, "In" );
    m_maxVolt = 5;

    addPropGroup( { tr("Main"), {
        new IntProp <ADC>("Num_Bits", tr("Size"),"_bits"
                         , this, &ADC::numOuts, &ADC::setNumOutputs, propNoCopy,"uint" ),

        new DoubProp<ADC>("Vref", tr("Reference Voltage"),"V"
                         , this, &ADC::maxVolt, &ADC::setMaxVolt ),
    },groupNoCopy} );

    addPropGroup( { tr("Electric"), IoComponent::outputProps()+IoComponent::outputType(),0 } );
    addPropGroup( { tr("Timing")  , IoComponent::edgeProps()                            ,0 } );
}
ADC::~ADC(){}

void ADC::stamp()
{
    m_inPin[0]->changeCallBack( this );
    LogicComponent::stamp();
}

void ADC::voltChanged()
{
    double volt = m_inPin[0]->getVoltage();
    m_nextOutVal = volt*m_maxValue/m_maxVolt+0.1;
    if( m_nextOutVal > m_maxValue ) m_nextOutVal = m_maxValue;
    LogicComponent::scheduleOutPuts( this );
}

void ADC::setNumOutputs( int outs )
{
    if( outs < 1 ) return;
    m_maxValue = pow( 2, outs )-1;
    IoComponent::setNumOuts( outs, "D" );
    updtInPins();
}
