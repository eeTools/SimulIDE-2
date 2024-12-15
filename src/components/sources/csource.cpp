/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QtMath>

#include "csource.h"
#include "simulator.h"
#include "circuitwidget.h"
#include "propdialog.h"
#include "pin.h"

#include "doubleprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("Csource",str)

listItem_t Csource::libraryItem(){
    return {
        tr("Controlled Source"),
        "Sources",
        "csource.png",
        "Csource",
        [](QString id){ return (Component*)new Csource( id ); } };
}

Csource::Csource( QString id )
       : Component( id )
       , Resistance( id )
{
    m_area = QRect(-16,-16, 32, 32 );

    m_nodes.resize(2); // Nodes 0 & 1  created in Resistance
    m_nodes[2] = -1;
    m_nodes[3] = -1;

    m_pin.resize(4);
    // Control Pins
    m_pin[0] = new Pin( 180, QPoint(-24,-8 ), id+"-cpPin", this);
    m_pin[1] = new Pin( 180, QPoint(-24, 8 ), id+"-cmPin", this);
    m_pin[0]->setFontSize( 9 );
    m_pin[0]->setSpace( 1.7 );
    m_pin[0]->setLabelText("+");
    m_pin[0]->setLabelColor( Qt::red );
    m_pin[1]->setFontSize( 9 );
    m_pin[1]->setSpace( 1.7 );
    m_pin[1]->setLabelText("–");  // U+2013
    m_pin[1]->setLabelColor( QColor( 0, 0, 0 ) );

    // Source Pins
    m_pin[2] = new Pin(  90, QPoint( 0,-20 ), id+"-s1Pin", this);
    m_pin[3] = new Pin( 270, QPoint( 0, 20 ), id+"-s2Pin", this);
    m_pin[2]->setFlag( QGraphicsItem::ItemStacksBehindParent, false );
    m_pin[3]->setFlag( QGraphicsItem::ItemStacksBehindParent, false );

    m_controlPins = true;
    m_currSource  = true;
    m_currControl = false;

    m_gain = 1;
    m_voltage = 5;
    m_current = 1;

    setLabelPos( 4,-28, 0 );
    setValLabelPos( 4, 18, 0 ); // x, y, rot

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"), {
        new BoolProp<Csource>("Control_Pins", tr("Use Control Pins"), ""
                             , this, &Csource::controlPins, &Csource::setControlPins, propNoCopy ),

        new BoolProp<Csource>("CurrSource", tr("Current Source"),""
                             , this, &Csource::currSource , &Csource::setCurrSource ),

        new BoolProp<Csource>("CurrControl", tr("Current Controlled"), ""
                             , this, &Csource::currControl, &Csource::setCurrControl, propNoCopy ),

        new DoubProp<Csource>("Gain", tr("Gain"), ""
                             , this, &Csource::gain, &Csource::setGain ),

        new DoubProp<Csource>("Voltage", tr("Voltage"), "V"
                             , this, &Csource::volt, &Csource::setVolt ),

        new DoubProp<Csource>("Current", tr("Current"), "A"
                             , this, &Csource::outCurrent , &Csource::setOutCurrent )
    }, 0 } );
}
Csource::~Csource() {}

void Csource::initialize()
{
    if( m_currControl ) m_admitance = 1/cero_doub;
    else                m_admitance = cero_doub;

    for( int i=0; i<4; ++i ) m_nodes[i] = m_pin[i]->getNode();


    //eResistor::stamp();

    m_lastCurr = 0;
    m_changed = true;
    updateStep();
}

void Csource::voltChanged()
{
    double volt = m_pin[0]->getVoltage() - m_pin[1]->getVoltage();
    setVoltage( volt );
}

void Csource::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    updtProperties();

    m_pin[0]->setEnabled( m_controlPins && !m_linkedTo );
    m_pin[0]->setVisible( m_controlPins && !m_linkedTo );
    m_pin[1]->setEnabled( m_controlPins && !m_linkedTo );
    m_pin[1]->setVisible( m_controlPins && !m_linkedTo );

    if( m_currControl )
    {
        m_admitance = 1/cero_doub;
        m_pin[0]->setLabelText( "" );
        m_pin[1]->setLabelText( "" );
    }else{
        m_admitance = cero_doub;
        m_pin[0]->setLabelText("+");
        m_pin[1]->setLabelText("–");  // U+2013
    }
    //v::stampAdmit();
/*
    if( m_currSource )
    {
        m_pin[2]->stampAdmitance( 0 );
        m_pin[3]->stampAdmitance( 0 );
    }else{
        m_pin[2]->stampAdmitance( 1/cero_doub );
        m_pin[3]->stampAdmitance( 1/cero_doub );
    }

    if( !m_controlPins && !m_linkedTo )
    {
        m_pin[0]->removeConnector();
        m_pin[1]->removeConnector();

        if( m_currSource )
        {
            m_pin[2]->stampCurrent(-m_current );
            m_pin[3]->stampCurrent( m_current );
        }else{
            m_pin[2]->stampCurrent( m_voltage/cero_doub );
            m_pin[3]->stampCurrent(-m_voltage/cero_doub );
        }
    }
    else voltChanged();

    bool connected = m_pin[0]->isConnected() && m_pin[1]->isConnected();
    m_pin[0]->changeCallBack( this, connected && m_controlPins );
    m_pin[1]->changeCallBack( this, connected && m_controlPins );

    */
    update();
}

void Csource::setVoltage( double v )
{
    double current = v;
    //if( qFabs( curr - m_lastCurr ) < 1e-5 ) return;
    m_lastCurr = current;

    if     ( m_currSource ) current = -current;   // Current source
    else if( current != 0 ) current /= cero_doub; // Voltage source

    if( m_currControl  ) current *= m_admitance;   // Current controlled

    current *= m_gain;
    ///m_pin[2]->stampCurrent( current );
    ///m_pin[3]->stampCurrent(-current );
}

bool Csource::setLinkedTo( Linker* li )
{
    bool linked = Component::setLinkedTo( li );
    setControlPins( m_controlPins );
    return linked;
}

void Csource::setLinkedValue( double v, int i )
{
    if( m_currControl ) v /= m_admitance;
    setVoltage( v );
}

void Csource::setGain( double g )
{
    m_gain = g;
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void Csource::setVolt( double volt )
{
    if( volt < 0 ) volt = 0;
    m_voltage = volt;
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void Csource::setOutCurrent( double c )
{
    if( c < 0 ) c = 0;
    m_current = c;
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void Csource::setCurrSource( bool c )
{
    m_currSource = c;
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

 void Csource::setCurrControl( bool c )
 {
     m_currControl = c;
     m_changed = true;
     if( !Simulator::self()->isRunning() ) updateStep();
 }

void Csource::setControlPins( bool set )
{
    int length = (set || m_linkedTo) ? 8 : 10;
    m_pin[2]->setLength( length );
    m_pin[3]->setLength( length );

    m_controlPins = set;
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void Csource::updtProperties()
{
    if( !m_propDialog ) return;
    bool controlled = m_controlPins || m_linkedTo; // Controlled by pins or Linked

    m_propDialog->showProp("Voltage"    , !controlled && !m_currSource );
    m_propDialog->showProp("Current"    , !controlled &&  m_currSource );
    m_propDialog->showProp("CurrControl",  controlled );
    m_propDialog->showProp("Gain"       ,  controlled );

    m_propDialog->adjustWidgets();
}

void Csource::slotProperties()
{
    Component::slotProperties();
    updtProperties();
}

void Csource::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    QPen pen = p->pen();

    if( m_controlPins && !m_linkedTo )
    {
        pen.setWidth(1);
        p->setPen(pen);
        p->drawRect( m_area );
    }
    pen.setWidth(2);
    p->setPen(pen);

    if( !m_controlPins && !m_linkedTo )
    {
        p->drawEllipse(-10,-10, 20, 20 );
    }
    else{
        QPainterPath path;
        QVector<QPointF> points;
        points << QPointF(-8, 0 )
               << QPointF( 0,-13 )
               << QPointF( 8, 0 )
               << QPointF( 0, 13 );

        path.addPolygon( QPolygonF(points) );
        path.closeSubpath();
        p->drawPath( path );
    }

    pen.setWidth(1);
    p->setPen(pen);
    if( m_currSource )
    {
        p->drawLine( 0,-5, 0, 5 );
        p->drawLine(-2, 2, 0, 5 );
        p->drawLine( 2, 2, 0, 5 );
    }else{
        p->drawLine(-2,-4, 2,-4 );
        p->drawLine( 0,-6, 0,-2 );
        p->drawLine(-2, 4, 2, 4 );
    }
    if( m_currControl && m_controlPins && !m_linkedTo )
    {
        pen.setWidthF(0.6);
        p->setPen(pen);
        p->drawLine(-12,-7,-12, 7 );
        p->drawLine(-13, 5,-12, 7 );
        p->drawLine(-11, 5,-12, 7 );
    }
    Component::paintSelected( p );
}
