/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QAbstractSlider>
#include <QGraphicsProxyWidget>

#include "m_dial.h"
#include "fblock.h"
#include "fcomponent.h"
#include "dialwidget.h"

#include "boolprop.h"
#include "intprop.h"
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

    m_minVal = 0;
    m_maxVal = 999;
    m_steps  = 1000;

    m_dialW = nullptr;

    addPropGroup( { tr("Main"),
    {
        new IntProp<mDial>("Min_Val", tr("Minimum Value"), ""
                         , this, &mDial::minVal, &mDial::setMinVal ),

        new IntProp<mDial>("Max_Val", tr("Maximum Value"), ""
                         , this, &mDial::maxVal, &mDial::setMaxVal ),

        new IntProp<mDial>("Steps", tr("Steps"), ""
                         , this, &mDial::steps, &mDial::setSteps )
    },0 } );

    addPropGroup( { "Dial",
    {
        new PointProp<mDial>("pos", tr("Position"),""
                            , this, &mDial::position, &mDial::setPosition ),

        new DoubProp<mDial>("angle", tr("Angle") ,""
                           , this, &mDial::angle , &mDial::setAngle ),

        new DoubProp<mDial>("scale", tr("Scale") ,""
                           , this, &mDial::scale , &mDial::setScale ),

        new BoolProp<mDial>("slider", tr("Slider"),""
                           , this, &mDial::slider, &mDial::setSlider )
    },0} );
}
mDial::~mDial(){}

void mDial::setComponent( fComponent* c )
{
    Module::setComponent( c );

    m_dialW = new DialWidget();
    QObject::connect( m_dialW->dial(), &QAbstractSlider::valueChanged, [=](int v){ dialChanged(v); } );

    m_proxy = m_component->addWidget( m_dialW );
    m_proxy->setTransformOriginPoint( QPoint( m_dialW->width()/2, m_dialW->height()/2 ) );
    m_slider = false;
    updateProxy();
}

void mDial::initModule()
{
    m_output = 0;
}

void mDial::runStep()
{
    if( !m_modChanged ) return;
    m_modChanged = false;

    m_output = m_dialW->value();
    m_outSignal.changed();
}

void mDial::setMinVal( int min )
{
    if( min < 0    ) min = 0;
    if( min > m_maxVal ) min = m_maxVal;
    m_minVal = min;

    m_modChanged = true;
}

void mDial::setMaxVal( int max )
{
    if( max < 0 ) max = 0;
    if( max < m_minVal ) max = m_minVal;
    m_maxVal = max;

    m_modChanged = true;
}

void mDial::setSteps( int s )
{
    if( s < 2 ) s = 2;
    if( m_steps == s ) return;
    m_steps = s;
    m_dialW->setMaximum( s-1 );

    int single = s/40;
    if( single < 1 ) single = 1;
    m_dialW->setSingleStep( single );
}

void mDial::setSlider( bool s )
{
    m_slider = s;

    /// QObject::disconnect( m_dialW.dial(), &QAbstractSlider::valueChanged );

    m_dialW->setType( s ? 1: 0 );

    QObject::connect( m_dialW->dial(), &QAbstractSlider::valueChanged, [=](int v){ dialChanged(v); } );

    updateProxy();
}

double mDial::scale() { return m_dialW->scale(); }

void mDial::setScale( double s )
{
    if( s <= 0 ) return;
    m_dialW->setScale(s);
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
    m_modChanged = true;
    //if( !Simulator::self()->isRunning() ) updateStep();
}

void mDial::updateProxy()
{
    m_proxy->setPos( m_position+QPoint(-m_dialW->width()/2,-m_dialW->height()/2) );
}
