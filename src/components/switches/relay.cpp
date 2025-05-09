/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <math.h>

#include "relay.h"
#include "simulator.h"
#include "circuit.h"
#include "e-node.h"

#include "doubleprop.h"
#include "boolprop.h"
#include "intprop.h"

#define tr(str) simulideTr("Relay",str)

listItem_t Relay::libraryItem(){
    return {
        tr( "Relay" ),
        "Switches",
        "relay-spst.png",
        "Relay",
        [](QString id){ return (Component*)new Relay("Relay", id ); } };
}

Relay::Relay( QString type, QString id )
     : MechContact( type, id )
{
    m_ePin.resize(4);
    m_pin.resize(2);

    m_pin0 = 4;

    m_inductor = new Inductor( "Inductor", m_id+"-inductor" );
    m_inductor->setParentItem( this );
    m_inductor->moveTo( QPointF( 0, 0 ) );
    m_inductor->setValue( 0.1 );  // 100 mH
    m_inductor->setResist( 100 );
    m_inductor->setShowVal( false );

    m_pin[0] = m_inductor->getPin( 0 );
    m_pin[1] = m_inductor->getPin( 1 );
    addSignalPin( m_pin[0] );
    addSignalPin( m_pin[1] );

    m_trigCurrent = 0.02;
    m_relCurrent  = 0.01;

    setValLabelPos(-16, 6, 0);
    setLabelPos(-16, 8, 0);

    SetupSwitches( 1, 1 );

    addPropGroup( { tr("Main"), {
        new BoolProp<Relay>("Norm_Close", tr("Normally Closed"),""
                           , this, &Relay::nClose, &Relay::setNClose ),

        new BoolProp<Relay>("DT", tr("Double Throw"),""
                           , this, &Relay::dt, &Relay::setDt, propNoCopy ),

        new IntProp <Relay>("Poles", tr("Poles"),""
                           , this, &Relay::poles, &Relay::setPoles, propNoCopy,"uint" ),
    }, 0} );

    addPropGroup( { tr("Electric"), {
        new DoubProp<Relay>("IOn" , tr("IOn"),"mA"
                           , this, &Relay::iTrig, &Relay::setITrig),

        new DoubProp<Relay>("IOff", tr("IOff"),"mA"
                           , this, &Relay::iRel, &Relay::setIRel )
    }, 0} );

    addPropGroup( { tr("Coil"), {
        new DoubProp<Inductor>("Inductance", tr("Inductance"),"mH"
                              , m_inductor, &Inductor::value , &Inductor::setValue ),

        new DoubProp<Inductor>("Rcoil", tr("Resistance"),"Ω"
                              , m_inductor, &Inductor::resist, &Inductor::setResist),

        new DoubProp<Inductor>("ReaStep", tr("Reactive Step"),"ns"
                              ,m_inductor, &Inductor::reaStep, &Inductor::setReaStep,0,"uint" )
    }, 0} );
}
Relay::~Relay(){}

void Relay::initialize()
{
    m_relayOn = false;

    if( Simulator::self()->isRunning() )
        m_internalEnode = new eNode( m_id+"-internaleNode" );
}

void Relay::stamp()
{
    MechContact::stamp();

    m_inductor->getPin( 0 )->changeCallBack( this );
    m_inductor->getPin( 1 )->changeCallBack( this );
}

void Relay::voltChanged()
{
    double indCurr = fabs( m_inductor->indCurrent() );
    bool relayOn;

    if( m_relayOn ) relayOn = ( indCurr > m_relCurrent );
    else            relayOn = ( indCurr > m_trigCurrent );
    m_relayOn = relayOn;

    if( m_nClose ) relayOn = !relayOn;
    if( relayOn != m_closed ) setSwitch( relayOn );
}

void Relay::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    p->drawRect( m_area );

    MechContact::paint( p, o, w );
}
