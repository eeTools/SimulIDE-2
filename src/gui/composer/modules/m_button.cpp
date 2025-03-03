/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <QAbstractSlider>
#include <QGraphicsProxyWidget>

#include "m_button.h"
#include "fblock.h"
#include "fcomponent.h"
#include "custombutton.h"
#include "simulator.h"

#define tr(str) simulideTr("mButton",str)

listItem_t mButton::registerItem(){
    return {
        "Button",
        "Controls",
        "button.png",
        "mButton",
        [](int id){ return (CompBase*)new mButton( id ); } };
}

mButton::mButton( int id )
       : Module( id )
       , m_outSignal("output", hookOutputInt )
{
    m_outSignal.setIntData( &m_output );
    m_signals.emplace_back( &m_outSignal );

    m_button = nullptr;
    m_width  = 16;
    m_height = 16;

    Simulator::self()->addToUpdateList( this );

    /*addPropGroup( { tr("Main"),
    {

    },0 } );

    addPropGroup( { "Dial",
    {
        new PointProp("pos", tr("Position"),"", this ),
        new IntProp("width", tr("Width") ,"", this ),
        new IntProp("height", tr("Height") ,"", this ),
    },0} );*/
}
mButton::~mButton(){}

void mButton::setComponent( fComponent* c )
{
    Module::setComponent( c );

    m_button = new CustomButton( );
    m_button->setCheckable( true );
    QObject::connect( m_button, &CustomButton::clicked, [=](){ onbuttonclicked(); });

    m_proxy = m_component->addWidget( m_button );
    m_proxy->setTransformOriginPoint( QPoint( m_button->width()/2, m_button->height()/2 ) );

    updateProxy();
}

void mButton::initModule()
{
    m_changed = true;
    updateStep();
}

void mButton::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    m_output = m_button->isChecked() ? 1 : 0;
    m_outSignal.changed();

    m_component->voltChanged();
}

void mButton::setWidth( int width )
{
    m_width = width;
    m_button->setFixedWidth( width );
    updateProxy();
}

void mButton::setHeight( int height )
{
    m_height = height;
    m_button->setFixedHeight( height );
    updateProxy();
}

void mButton::setPosition( QPointF p )
{
    m_position = p.toPoint();
    updateProxy();
}

void mButton::onbuttonclicked()
{
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void mButton::updateProxy()
{
    m_proxy->setPos( m_position+QPoint(-m_button->width()/2,-m_button->height()/2) );
}
