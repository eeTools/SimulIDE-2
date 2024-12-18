/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QAbstractSlider>
#include <QGraphicsProxyWidget>

#include "m_dial.h"
#include "fblock.h"
#include "fcomponent.h"

#include "boolprop.h"
#include "doubleprop.h"
#include "pointprop.h"

#define tr(str) simulideTr("mDial",str)

listItem_t mDial::registerItem(){
    return {
        "Dial",
        "Controls",
        "dial.png",
        "mDial",
        [](QString id){ return (CompBase*)new mDial( id ); } };
}

mDial::mDial( QString name )
     : Module( name )
     , m_outSignal("output", hookOutputInt )
{
    m_outSignal.setIntData( &m_output );
    m_signals.emplace_back( &m_outSignal );

    m_position = QPoint( 0, 0 );
    m_angle = 0;

    QObject::connect( m_dialW.dial(), &QAbstractSlider::valueChanged, [=](int v){ dialChanged(v); } );

    //Simulator::self()->addToUpdateList( this );
    addPropGroup( { "Main",
    {
        new PointProp<mDial>("pos", tr("Position"),""
                            , this, &mDial::position, &mDial::setPosition ),

        new DoubProp<mDial>("angle", tr("Angle") ,""
                            , this, &mDial::angle , &mDial::setAngle )
    },0} );
    addPropGroup( { "Dial",
    {
        new BoolProp<mDial>("slider", tr("Slider"),""
                            , this, &mDial::slider, &mDial::setSlider ),

        new DoubProp<mDial>("scale", tr("Scale") ,""
                            , this, &mDial::scale , &mDial::setScale )
    },0} );
}
mDial::~mDial(){}

void mDial::setComponent( fComponent* c )
{
    Module::setComponent( c );

    m_proxy = m_component->addWidget( &m_dialW );
    m_slider = false;
}

void mDial::initModule()
{
    m_output = 0;
}

void mDial::runStep()
{
    if( !m_changed ) return;
    m_changed = false;

    int out = 0;

    if( m_output == out ) return;
    m_output = out;
    m_outSignal.changed();
}

void mDial::setSlider( bool s )
{
    m_slider = s;

    /// QObject::disconnect( m_dialW.dial(), &QAbstractSlider::valueChanged );

    m_dialW.setType( s ? 1: 0 );

    //QObject::connect( m_dialW.dial(), &QAbstractSlider::valueChanged, [=](int v){ dialChanged(v); } );

    updateProxy();
}

void mDial::setScale( double s )
{
    if( s <= 0 ) return;
    m_dialW.setScale(s);
    updateProxy();
}

void mDial::setAngle( double a )
{
    m_angle = a;
    m_proxy->setRotation( a );
    updateProxy();
}

void mDial::setPosition( QPointF p )
{
    m_position = p.toPoint();
    updateProxy();
}

void mDial::dialChanged( int ) // Called when dial is rotated
{
    m_needUpdate = true;
    //if( !Simulator::self()->isRunning() ) updateStep();
}

void mDial::updateProxy()
{
    m_proxy->setPos( m_position+QPoint(-m_dialW.width()/2,-m_dialW.height()-5) );
}
