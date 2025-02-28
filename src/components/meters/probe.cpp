/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QtMath>
#include <QPainter>
#include <QMenu>

#include "probe.h"
#include "wire.h"
#include "wireline.h"
#include "circuitwidget.h"
#include "circuit.h"
#include "simulator.h"
#include "label.h"
#include "iopin.h"

#define tr(str) simulideTr("Probe",str)

listItem_t Probe::registerItem(){
    return {
        tr("Probe"),
        "Meters",
        "probe.png",
        "Probe",
        [](int id){ return (CompBase*)new Probe( id ); } };
}

Probe::Probe( int id )
     : Component( id )
     , Element()
     , m_threshold( this, "thres", tr("Threshold")   ,"V", 2.5, 0 )
     , m_showVolt ( this, "showv", tr("Show Voltage"), true,  P_NoSet | P_NoCopy )
     , m_small    ( this, "small", tr("Small size")  , false, P_NoSet  )
     , m_pause    ( this, "pause", ""                , false, P_Hidden )
{
    setZValue( 200 );
    m_area = QRect(-12,-8, 20, 16 );

    m_graphical = true;
    //m_pause.set( false );
    //m_showVolt.set( true );

    /*auto sm = [this](bool s){ this->setSmall(s); };
    sm( false );
    m_small.addSetter( sm );*/

    //m_voltTrig = 2.5;
    m_voltIn = 0;
    m_voltStr = "0 V";
    m_valLabel->setLine( m_showVolt.idInt(), m_voltStr );

    m_inputPin = new InputPin( 180, QPoint(-22,0), "inPin@"+QString::number(id), this );
    m_inputPin->setBoundingRect( QRect(-1, -1, 2, 2) );
    m_inputPin->setInputImp( 1e9 );
    m_pin << m_inputPin;

    setValLabelPos( 16, 0, 45 ); // x, y, rot
    setLabelPos( 16,-16, 45 );
    setRotation( rotation() - 45 );
    m_voltIn = -1; // Force update
    setVolt( 0 );

    setSmall( false );

    Simulator::self()->addToUpdateList( this );

    /*addPropGroup( { tr("Main"), {}, 0 },
    {
        {"threshold", tr("Threshold")   ,"V", &m_voltTrig, P_Double, 0        },
        {"showvolt" , tr("Show Voltage"), "", &m_showVolt, P_Bool  , P_NoCopy },
        {"small"    , tr("Small size")  , "", &m_small   , P_Bool  , P_NoSet  },
        {"pause"    , ""                , "", &m_pause   , P_Bool  , P_Hidden }
    });*/

    addPropGroup( { tr("Main"), { &m_threshold, &m_showVolt, &m_small, &m_pause }, 0 });
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
    if( !m_pause.get() ) return;

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

    if( !m_showVolt.get() ) return;
    if( qFabs(volt) < 0.01 ) volt = 0;
    
    float v = ( volt > 0 ) ? 0.5 : -0.5;
    v = float(int( v+volt*100 ))/100;

    QString voltStr = QString("%1 V").arg(v);
    m_valLabel->setLine( m_showVolt.idInt(), voltStr );

    m_voltStr = voltStr;
}

void Probe::rotateAngle( double a )
{
    Component::rotateAngle( a );
    m_idLabel->rotateAngle(-a );
    m_valLabel->rotateAngle(-a );
}

void Probe::setSmall( bool s )
{
    m_small.set( s );

    if( s ){
        m_inputPin->setLength( 6 );
        m_area = QRect(-16, -4, 8, 8 );
    }else{
        m_inputPin->setLength( 14 );
        m_area = QRect(-12,-8, 20, 16 );
    }
    update();
    Circuit::self()->update();
}

void Probe::slotBreakpoint()
{
    m_pause.set( !m_pause.get() );

    int node = m_inputPin->getNode();
    if( node >= 0 ) m_kcl->addChangeCB( this, node );

    update();
}

void Probe::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    if( m_inputPin->wire() )
    {
        QString iconStr = m_pause.get() ? ":/nobreakpoint.png" : ":/breakpoint.png";
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

void Probe::propertyChanged( const ComProperty* prop )
{
    if     ( prop == &m_small    ) setSmall( m_small.get() );
    else if( prop == &m_showVolt )
    {
        if( m_showVolt.get() ) updateStep();
        else                   m_valLabel->setLine( m_showVolt.idInt(), "" );

    }
    else Component::propertyChanged( prop );
}

void Probe::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    double threshold = m_threshold.get();

    if      ( m_voltIn >  threshold ) p->setBrush( QColor( 255, 166,   0 ) );
    else if ( m_voltIn < -threshold ) p->setBrush( QColor(   0, 100, 255 ) );
    else                              p->setBrush( QColor( 230, 230, 255 ) );

    if( m_pause.get() )
    {
        QPen pen = p->pen();
        pen.setWidthF( 2.5 );
        pen.setColor( QColor( 255, 0, 0 ));
        p->setPen(pen);
    }
    if( m_small.get() ) p->drawEllipse( m_area );
    else                p->drawEllipse( QRect(-8,-8, 16, 16 ) );

    Component::paintSelected( p );
}
