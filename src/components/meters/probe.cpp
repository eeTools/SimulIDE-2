/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "circuitwidget.h"
#include <QtMath>
#include <QPainter>
#include <QMenu>

#include "probe.h"
#include "wire.h"
#include "wireline.h"
#include "circuit.h"
#include "simulator.h"
#include "label.h"
#include "iopin.h"

#include "doubleprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("Probe",str)

listItem_t Probe::registerItem(){
    return {
        tr("Probe"),
        "Meters",
        "probe.png",
        "Probe",
        [](QString id){ return (CompBase*)new Probe( id ); } };
}

Probe::Probe( QString id )
     : Component( id )
     , Element( id )
{
    setZValue( 200 );
    m_area = QRect(-12,-8, 20, 16 );
    m_graphical = true;
    m_pauseState = false;

    m_voltTrig = 2.5;
    m_voltIn = 0;

    m_inputPin = new InputPin( 180, QPoint(-22,0), "inPin@"+id, this );
    m_inputPin->setBoundingRect( QRect(-1, -1, 2, 2) );
    m_inputPin->setInputImp( 1e9 );
    m_pin << m_inputPin;

    setValLabelPos( 16, 0, 45 ); // x, y, rot
    setShowVal( true );
    setLabelPos( 16,-16, 45 );
    setRotation( rotation() - 45 );
    m_voltIn = -1; // Force update
    setVolt( 0 );

    setSmall( false );

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"), {
        new BoolProp<Probe>("ShowVolt" , tr("Show Voltage"), ""
                           , this, &Probe::showVal, &Probe::setShowVal, propNoCopy ),

        new DoubProp<Probe>("Threshold", tr("Threshold"), "V"
                           , this, &Probe::threshold, &Probe::setThreshold ),

        new BoolProp<Probe>("Small", tr("Small size"), ""
                           , this, &Probe::isSmall, &Probe::setSmall ),

        new BoolProp<Probe>("Pause", "", ""
                           , this, &Probe::pauseState, &Probe::setPauseState, propHidden )
    }, 0 } );
}
Probe::~Probe(){}

void Probe::initialize()
{
    m_state = false;
    int node = m_inputPin->getNode();
    if( node >= 0 ) m_kcl->addChangeCB( this, node );
}

void Probe::updateStep()
{
    if( !Simulator::self()->isRunning() ) { setVolt( 0.0 ); return; }

    if( m_inputPin->isConnected() )// Voltage from connected pin
    {
         setVolt( m_inputPin->getVoltage() );
         return;
    }
    QList<QGraphicsItem*> list = m_inputPin->collidingItems(); // Voltage from connector or Pin behind inputPin
    if( list.isEmpty() ) { setVolt( 0.0 ); return; }

    for( QGraphicsItem* it : list )
    {
        if( it->type() == UserType+3 )                    // Pin found
        {
            Pin* pin =  qgraphicsitem_cast<Pin*>( it );
            setVolt( pin->getVoltage() );
            break;
        }else if( it->type() == UserType+2 )        // ConnectorLine
        {
            WireLine* line =  qgraphicsitem_cast<WireLine*>( it );
            Wire* wire = line->connector();
            setVolt( wire->getVoltage() );
            break;
}   }   }

void Probe::voltChanged()
{
    if( !m_pauseState ) return;

    bool state = m_inputPin->getLogicState();
    if( m_state == state ) return;
    m_state = state;
    CircuitWidget::self()->pauseCirc();
}

void Probe::setVolt( double volt )
{
    if( m_voltIn == volt ) return;
    m_voltIn = volt;
    update();       // Repaint

    if( !m_showVal ) return;
    if( qFabs(volt) < 0.01 ) volt = 0;
    
    float v = ( volt > 0 ) ? 0.5 : -0.5;
    v = float(int( v+volt*100 ))/100;
    setValLabelText( QString("%1 V").arg(v) );
}

void Probe::rotateAngle( double a )
{
    Component::rotateAngle( a );
    m_idLabel->rotateAngle(-a );
    m_valLabel->rotateAngle(-a );
}

void Probe::setSmall( bool s )
{
    m_small = s;

    if( s ){
        m_inputPin->setLength( 6 );
        m_area = QRect(-16, -4, 8, 8 );
    }else{
        m_inputPin->setLength( 14 );
        m_area = QRect(-12,-8, 20, 16 );
    }
    Circuit::self()->update();
}

void Probe::slotBreakpoint()
{
    m_pauseState = !m_pauseState;

    int node = m_inputPin->getNode();
    if( node >= 0 ) m_kcl->addChangeCB( this, node );

    update();
}

void Probe::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    if( m_inputPin->wire() )
    {
        QString iconStr = m_pauseState ? ":/nobreakpoint.png" : ":/breakpoint.png";
        QAction* breakAction = menu->addAction( QIcon( iconStr ),tr("Pause at state change") );
        QObject::connect( breakAction, &QAction::triggered, [=](){ slotBreakpoint(); } );
    }
    menu->addSeparator();
    Component::contextMenu( event, menu );
}

QPainterPath Probe::shape() const
{
    QPainterPath path;
    path.addEllipse( m_area );
    return path;
}

void Probe::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    if      ( m_voltIn > m_voltTrig)  p->setBrush( QColor( 255, 166, 0 ) );
    else if ( m_voltIn < -m_voltTrig) p->setBrush( QColor( 0, 100, 255 ) );
    else                              p->setBrush( QColor( 230, 230, 255 ) );

    if( m_pauseState )
    {
        QPen pen = p->pen();
        pen.setWidthF( 2.5 );
        pen.setColor( QColor( 255, 0, 0 ));
        p->setPen(pen);
    }
    if( m_small ) p->drawEllipse( m_area );
    else          p->drawEllipse( QRect(-8,-8, 16, 16 ) );

    Component::paintSelected( p );
}
