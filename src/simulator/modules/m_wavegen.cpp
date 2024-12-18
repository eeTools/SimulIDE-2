/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include "m_wavegen.h"

QString WaveGen::m_moduleType = "WaveGen";

moduleItem_t WaveGen::registerItem(){
    return {
        "Wave Generator",
        m_moduleType,
        "",
        "wavegen.png",
        [](QString id){ return (Module*)new WaveGen( id ); } };
}

WaveGen::WaveGen( QString name )
       : Module( name )
       , m_outSignal("output", hookOutputDoub )
{
    m_type = m_moduleType;
    m_waveType = SINE;

    m_outSignal.setDblData( &m_output );
    m_signals.emplace_back( &m_outSignal );


    m_waveList = QStringList()<<"Sine"<<"Saw"<<"Triangle"
                              <<"Square"<<"Random"<<"Wav";

    /*addPropGroup( { "Main",
    {
        new StrProp<WaveGen>("gType", "Type", m_bitOpList.join(",")
                         , this, &WaveGen::typeStr, &WaveGen::setTypeStr, propSlot, "enum" ),

        new IntProp<WaveGen>("size", "bits", ""
                         , this, &WaveGen::size, &WaveGen::setSize, propSlot ),

        new StrProp <WaveGen>("Trigger", "Trigger Type",""
                         , this, &WaveGen::triggerStr, &WaveGen::setTriggerStr, propNoCopy, "enum" ),
    },0} );*/
}
WaveGen::~WaveGen(){}

void WaveGen::initModule()
{
    m_output = 0;
}

void WaveGen::runStep()
{
    if( !m_changed ) return;
    m_changed = false;

    ///m_input = m_inputSlot.intData() & m_mask;
    //for( int i=0; i<m_bits; ++i )
    //    if( m_bitSlots.at(i)->intData() ) m_input |= 1<<i;

    //updateOutput();

     m_outSignal.changed();
}
