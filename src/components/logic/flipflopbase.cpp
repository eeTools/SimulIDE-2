/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "flipflopbase.h"
#include "simulator.h"
#include "circuit.h"
#include "iopin.h"

#include "boolprop.h"
#include "stringprop.h"

#define tr(str) simulideTr("FlipFlopBase",str)

FlipFlopBase::FlipFlopBase( QString type, QString id )
            : LogicComponent( type, id )
{
    m_dataPins = 0;
    m_useRS = true;

    addPropGroup( { tr("Main"), {
        new BoolProp<FlipFlopBase>("UseRS", tr("Use Set/Reset Pins"),""
                                  , this, &FlipFlopBase::pinsRS, &FlipFlopBase::usePinsRS, propNoCopy ),

        new StrProp <FlipFlopBase>("Trigger", tr("Trigger Type"),""
                                  , this, &FlipFlopBase::triggerStr,&FlipFlopBase::setTriggerStr, propNoCopy,"enum" ),
    }, groupNoCopy } );

    addPropGroup( { tr("Electric"), IoComponent::inputProps()+IoComponent::outputProps(),0 } );
    addPropGroup( { tr("Timing")  , IoComponent::edgeProps(),0 } );
}
FlipFlopBase::~FlipFlopBase(){}

bool FlipFlopBase::setPropStr( QString prop, QString val )
{
    if( prop =="Clock_Inverted" ) // Old circuits
    {
        m_clkPin->setInverted( val == "true" );
    }
    else if( prop =="Reset_Inverted" ) // Old circuits
    {
        bool invert = (val == "true");
        m_setPin->setInverted( invert );
        m_rstPin->setInverted( invert );
    }
    else return Component::setPropStr( prop, val );
    return true;
}

void FlipFlopBase::stamp()
{
    m_Q0 = (std::rand()%2);
    m_setPin->changeCallBack( this );
    m_rstPin->changeCallBack( this );

    if( m_trigger != Clock ) // J K or D
    { for( int i=0; i<m_dataPins; i++ ) m_inPin[i]->changeCallBack( this ); }

    LogicComponent::stamp();
    m_outPin[0]->setOutState( m_Q0 );
    m_outPin[1]->setOutState( !m_Q0 );
    m_nextOutVal = m_outValue = m_Q0? 1:2;
}

void FlipFlopBase::voltChanged()
{
    updateClock();  // Update Clk to don't miss any clock changes

    bool set   = sPinState();
    bool reset = rPinState();

    if( set || reset) m_nextOutVal = (set? 1:0) + (reset? 2:0);
    else if( m_clkState == Clock_Allow ) calcOutput();

    scheduleOutPuts( this );
}

void FlipFlopBase::usePinsRS( bool rs )
{
    m_useRS = rs;
    if( !rs ){
        m_setPin->removeWire();
        m_rstPin->removeWire();
    }
    m_setPin->setVisible( rs );
    m_rstPin->setVisible( rs );
}

bool FlipFlopBase::sPinState()
{
    if( m_useRS ) return m_setPin->getInpState();
    return false;
}

bool FlipFlopBase::rPinState()
{
    if( m_useRS ) return m_rstPin->getInpState();
    return false;
}
