/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>

#include "dac.h"
#include "simulator.h"
#include "iopin.h"

#include "doubleprop.h"
#include "boolprop.h"
#include "intprop.h"

#define tr(str) simulideTr("DAC",str)

listItem_t DAC::libraryItem(){
    return {
        tr("DAC"),
        "Other Logic",
        "3to1.png",
        "DAC",
        [](QString id){ return (Component*)new DAC("DAC", id ); } };
}

DAC::DAC( QString type, QString id )
   : LogicComponent( type, id )
{    
    m_width  = 4;
    m_height = 9;

    setLabelPos(-16,-80, 0);
    setNumInputs( 8 );       // Create Input Pins
    setNumOuts( 1, "Out" );
    m_maxVolt = 5;

    addPropGroup( { tr("Main"), {
        new IntProp <DAC>("Num_Bits", tr("Size"),"_bits"
                         , this, &DAC::numInps, &DAC::setNumInputs, propNoCopy,"uint" ),

        new DoubProp<DAC>("Vref", tr("Reference Voltage"),"V"
                         , this, &DAC::maxVolt, &DAC::setMaxVolt )
    }, groupNoCopy } );

    addPropGroup( { tr("Electric"),
        IoComponent::inputProps()
        +QList<ComProperty*>({
        new BoolProp<IoComponent>("Invert_Inputs", tr("Invert Inputs"),""
                                 , this, &IoComponent::invertInps, &IoComponent::setInvertInps, propNoCopy )})
    ,0} );

    addPropGroup( { tr("Timing"), IoComponent::edgeProps(),0 } );
}
DAC::~DAC(){}

void DAC::stamp()
{
    for( uint i=0; i<m_inPin.size(); ++i ) m_inPin[i]->changeCallBack( this );

    m_outPin[0]->setOutState( true );
    m_val = -1;
}

void DAC::voltChanged()
{
    m_val = 0;

    for( uint i=0; i<m_inPin.size(); ++i )
        if( m_inPin[i]->getInpState() ) m_val += pow( 2, i );

    Simulator::self()->addEvent( m_delayBase*m_delayMult, this );
}

void DAC::runEvent()
{
    double v = m_maxVolt*m_val/m_maxValue;

    m_outPin[0]->setOutHighV( v );
    m_outPin[0]->setOutState( true );
}

void DAC::setNumInputs( int inputs )
{
    if( inputs < 1 ) return;
    m_maxValue = pow( 2, inputs )-1;
    IoComponent::setNumInps( inputs, "D" );
    updtOutPins();
}
