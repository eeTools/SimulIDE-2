/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "fixedvolt.h"
#include "circuit.h"
#include "simulator.h"
#include "pin.h"
#include "custombutton.h"

#include "doubleprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("FixedVolt",str)

listItem_t  FixedVolt::libraryItem(){
    return {
        tr("Fixed Voltage"),
        "Sources",
        "voltage.png",
        "FixedVolt",
        [](QString id){ return (Component*)new FixedVolt( id ); } };
};

FixedVolt::FixedVolt( QString id )
         : Component( id )
         , m_out("outSour@"+id )
{
    m_area = QRect( -10, -10, 20, 20 );

    m_graphical = true;

    m_pin.emplace_back( new Pin( 0, QPoint(16,0), "Pin@"+id, this ) );
    m_out.setAdmitance( 1e9 );

    setValLabelPos(-16, 8 , 0 ); // x, y, rot 
    setLabelPos(-64,-24 );

    m_button = new CustomButton();
    m_button->setMaximumSize( 16,16 );
    m_button->setGeometry(-20,-16,16,16);
    m_button->setCheckable( true );

    m_proxy = Circuit::self()->addWidget( m_button );
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint(-32, -8) );

    setVoltage( 5.0 );
    
    Simulator::self()->addToUpdateList( this );

    QObject::connect( m_button, &CustomButton::clicked, [=](){ onbuttonclicked(); } );

    addPropGroup( { tr("Main"), {
        new DoubProp<FixedVolt>( "Voltage", tr("Voltage"),"V"
                               , this, &FixedVolt::voltage, &FixedVolt::setVoltage )
    }, 0} );

    addPropGroup( { "Hidden", {
        new BoolProp<FixedVolt>( "Out", "",""
                               , this, &FixedVolt::out, &FixedVolt::setOut ),
    }, groupHidden} );
}
FixedVolt::~FixedVolt(){}

void FixedVolt::initialize()
{
    m_out.setNode( m_pin[0]->getNode() );
    m_changed = true;
    //updateStep();
}

void FixedVolt::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;
    double volt = m_button->isChecked() ? m_voltage : 0;
    m_out.updtVoltage( volt );
    update();
}

void FixedVolt::onbuttonclicked()
{
    m_changed = true;
    //if( !Simulator::self()->isRunning() ) updateStep();
}

/*void FixedVolt::updateOutput()
{
    //m_outpin->setOutHighV( m_voltage );
    //m_changed = true;
}*/

bool FixedVolt::out() { return m_button->isChecked(); }

void FixedVolt::setOut( bool out )
{
    m_button->setChecked( out );
    onbuttonclicked();
}

void FixedVolt::setVoltage( double v )
{
    m_voltage = v;
    //m_outpin->setOutHighV( m_voltage );
    m_changed = true;
}

void FixedVolt::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    if( m_hidden ) return;

    Component::paint( p, o, w );

    if( m_button->isChecked() ) p->setBrush( QColor( 255, 166, 0 ) );
    else                        p->setBrush( QColor( 230, 230, 255 ) );

    p->drawRoundedRect( QRectF( -8, -8, 16, 16 ), 2, 2);

    Component::paintSelected( p );
}
