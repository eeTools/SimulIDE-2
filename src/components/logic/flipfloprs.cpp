/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "flipfloprs.h"
#include "simulator.h"
#include "iopin.h"

#define tr(str) simulideTr("FlipFlopRS",str)

listItem_t FlipFlopRS::libraryItem(){
    return {
        tr("FlipFlop RS"),
        "Memory",
        "2to2.png",
        "FlipFlopRS",
        [](QString id){ return (Component*)new FlipFlopRS("FlipFlopRS", id ); } };
}

FlipFlopRS::FlipFlopRS( QString type, QString id )
          : FlipFlopBase( type, id )
{
    m_width  = 3;
    m_height = 4;

    init({         // Inputs:
            "IL01S",
            "IL03R",
            "IL02>",
                   // Outputs:
            "OR01Q",
            "OR03!Q",
        });

    m_setPin = m_inPin[0];
    m_rstPin = m_inPin[1];
    m_clkPin = m_inPin[2];

    m_setPin->setInverted( true );
    m_rstPin->setInverted( true );

    setTrigger( Clock );

    remProperty("UseRS");
}
FlipFlopRS::~FlipFlopRS(){}

void FlipFlopRS::voltChanged()
{
    updateClock();
    bool clkAllow = (m_clkState == Clock_Allow); // Get Clk to don't miss any clock changes
    if( !clkAllow ) return;

    bool set   = sPinState();
    bool reset = rPinState();

    if( set || reset)
    {
        m_nextOutVal = (set? 1:0) + (reset? 2:0);
        scheduleOutPuts( this );
    }
}
