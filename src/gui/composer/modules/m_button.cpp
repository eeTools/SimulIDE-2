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

//#include "boolprop.h"
#include "intprop.h"
//#include "doubleprop.h"
#include "pointprop.h"

#define tr(str) simulideTr("mButton",str)

listItem_t mButton::registerItem(){
    return {
        "Button",
        "Controls",
        "button.png",
        "mButton",
        [](QString id){ return (CompBase*)new mButton( id ); } };
}

mButton::mButton( QString name )
       : Module( name )
       , m_outSignal("output", hookOutputInt )
{
    m_outSignal.setIntData( &m_output );
    m_signals.emplace_back( &m_outSignal );

    m_button = nullptr;
    m_width  = 16;
    m_height = 16;

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"),
    {

    },0 } );

    addPropGroup( { "Dial",
    {
        new PointProp<mButton>("pos", tr("Position"),""
                            , this, &mButton::position, &mButton::setPosition ),

        new IntProp<mButton>("width", tr("Width") ,""
                           , this, &mButton::width , &mButton::setWidth ),

        new IntProp<mButton>("height", tr("Height") ,""
                           , this, &mButton::height , &mButton::setHeight ),
    },0} );
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
    m_output = 0;
    m_outSignal.changed();
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
}

void mButton::updateProxy()
{
    m_proxy->setPos( m_position+QPoint(-m_button->width()/2,-m_button->height()/2) );
}
