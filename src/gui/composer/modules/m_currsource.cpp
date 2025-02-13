/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

//#include <QStringList>
//#include <math.h>

#include "m_currsource.h"
#include "fblock.h"
#include "pin.h"

#include "doubleprop.h"

#define tr(str) simulideTr("mCurrentSource",str)

listItem_t mCurrentSource::registerItem(){
    return {
        "Curren Source",
        "Sources",
        "gate.png",
        "mCurrentSource",
        [](int id){ return (CompBase*)new mCurrentSource( id ); } };
}

mCurrentSource::mCurrentSource( int id )
              : Module( id )
              , m_inputSlot("input", hookInputDoub )
{
    m_slots.emplace_back( &m_inputSlot );

    m_gain = 1;

    addPropGroup( { "Main",
    {
        new DoubProp<mCurrentSource>("gain", tr("Gain"), ""
                                    , this, &mCurrentSource::gain, &mCurrentSource::setGain ),
    },0} );
}
mCurrentSource::~mCurrentSource(){}

void mCurrentSource::setComponent( fComponent* c )
{
    Module::setComponent( c );

    m_pPin = new Pin( 180, QPoint(-16, 0 ), "pPin@"+m_id, (Component*)m_component );
    m_nPin = new Pin( 0,   QPoint( 16, 0 ), "nPin@"+m_id, (Component*)m_component );
}

void mCurrentSource::initModule()
{
    m_modChanged = false;
    m_input = 0;
    m_current = 0;
}

void mCurrentSource::runStep()
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    m_input = m_inputSlot.dblData();

    double current = m_input*m_gain;
    if( m_current == current ) return;
    m_current = current;

    //m_pPin->updateCurrent( current );
    //m_nPin->updateCurrent(-current );
}
