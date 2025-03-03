/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QInputDialog>

#include "tunnel.h"
#include "linker.h"
#include "propdialog.h"
#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"
#include "utils.h"

#define tr(str) simulideTr("Tunnel",str)

QHash<QString, QList<Tunnel*>*> Tunnel::m_tunnels;

listItem_t Tunnel::registerItem(){
    return {
        tr("Tunnel."),
        "Connectors",
        "tunnel.png",
        "Tunnel",
        [](int id){ return (CompBase*)new Tunnel( id ); } };
}

Tunnel::Tunnel( int id )
      : Component( id )
{
    m_size = 20;
    m_blocked = false;
    m_packed  = false;
    m_show    = false;
    m_name = "";

    m_pin << new Pin( 0, QPoint(0,0), "pin@"+id, this);
    m_pin[0]->setLabelColor( Qt::black );
    m_pin[0]->setLength( 5 );
    m_pin[0]->setSpace( 4 );

    setLabelPos(-16,-24, 0);
    //setRotated( false );

    /*addPropGroup( { tr("Main"), {}, groupNoCopy },
    {
        {"tid", tr("Id") , "", &m_name, P_String, P_NoSet },
        {"bus", tr("Bus"), "", &m_isBus, P_Bool , P_NoSet }
    });*/

    /*addPropGroup( {"Hidden", {}, groupNoCopy | groupHidden},
    {
        {"Uid","","", nullptr, P_String, 0 }
    });*/
}
Tunnel::~Tunnel() {}

int Tunnel::getEnode( QString n ) // Static
{
    QList<Tunnel*>* list = m_tunnels.value( n );
    if( !list ) return -1;
    Tunnel* tunnel= list->first();
    if( tunnel ) return tunnel->getPin()->getNode();
    return -1;
}

void Tunnel::clearTunnels() // Static
{
    m_tunnels.clear();
}

void Tunnel::setEnode( int node, int n )
{
    if( m_blocked ) return;
    m_blocked = true;

    if( node ) m_pin[0]->registerPinsW( node, n );
    m_blocked = false;
}

void Tunnel::registerEnode( int enode, int n ) // called by m_pin[0]
{
    if( m_blocked ) return;

    QList<Tunnel*>* list = m_tunnels.value( m_name );
    if( !list ) return;

    m_blocked = true;

    for( Tunnel* tunnel: *list )
        if( tunnel != this ) tunnel->setEnode( enode, n );

    m_blocked = false;
}

void Tunnel::setName( QString name )
{
    setGroupName( name, true );
}

void Tunnel::setGroupName( QString name, bool single )
{
    if( name == m_name ) return;
    if( single && Simulator::self()->isRunning() )  CircuitWidget::self()->powerCircOff();

    removeTunnel(); // Remove old name before setting new one

    m_name = name;
    if( !m_packed ) m_pin[0]->setLabelText( name, false );
    if( name == "" ) m_size = 20;
    else  m_size = snapToGrid( m_pin[0]->labelSizeX()+4 );
    //setRotated( m_rotated );

    if( m_tunnels.contains( name ) ) // There is already tunnel with this name
    {
        QList<Tunnel*>* list = m_tunnels.value( name );
        if( !list->contains( this ) ) list->append( this );
    }
    else   // name doesn't exist: Create a new List for this name
    {
        QList<Tunnel*>* list = new QList<Tunnel*>();
        list->append( this );
        m_tunnels[name] = list;
    }
    if( single ) Circuit::self()->update();
    if( m_propDialog ) m_propDialog->updtValues();
}

/*bool Tunnel::isBus()
{
    return m_pin[0]->wireFlags() & wireBus;
}

void Tunnel::setIsbus( bool b )
{
    m_pin[0]->writeWireFlag( wireBus, b );
}*/

void Tunnel::setPacked( bool p )
{
    m_packed = p;
    if( p ) m_pin[0]->setLength( 8 );
    else    m_pin[0]->setLength( 5 );
    m_pin[0]->setSpace( 8-m_pin[0]->length() );
}

void Tunnel::removeTunnel()
{
    if( m_name.isEmpty() ) return;

    QList<Tunnel*>* list = m_tunnels.value( m_name );
    if( !list ) return;

    list->removeAll( this );
    if( !list->isEmpty() ) return;

    m_tunnels.remove( m_name );
    delete list;
}

void Tunnel::remove()
{
    if( Simulator::self()->isRunning() )  CircuitWidget::self()->powerCircOff();

    removeTunnel();
    Component::remove();
    Circuit::self()->update();
}

void Tunnel::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    if( m_show )
    {
        QAction* hideAction = menu->addAction( QIcon(":nobreakpoint.png"),tr("Hide group") );
        QObject::connect( hideAction, &QAction::triggered, [=](){ hideGroup(); } );
    }else{
        QAction* showAction = menu->addAction( QIcon(":/breakpoint.png"),tr("Show group") );
        QObject::connect( showAction, &QAction::triggered, [=](){ showGroup(); } );
    }
    QAction* nameAction = menu->addAction( QIcon(":/rename.svg"),tr("Rename group") );
    QObject::connect( nameAction, &QAction::triggered, [=](){ renameGroup(); } );

    menu->addSeparator();
    Component::contextMenu( event, menu );
}

void Tunnel::renameGroup()
{
    bool ok;
    QString text = QInputDialog::getText( nullptr, tr("Rename Tunnels"),
                                         tr("New name:"), QLineEdit::Normal,
                                         m_name, &ok );
    if( ok && !text.isEmpty() )
    {
        QList<Tunnel*>* list = m_tunnels.value( m_name );
        if( !list ) return;
        QVector<Tunnel*> tunnels = list->toVector();
        for( Tunnel* tunnel: tunnels ) tunnel->setGroupName( text, false );

        Circuit::self()->update();
    }
}

void Tunnel::showGroup()
{
    for( QList<Tunnel*>* list : m_tunnels.values() )  // Hide other groups
    {
        if( !list ) continue;
        for( Tunnel* tunnel: *list ) tunnel->m_show = false;
    }
    showHide( true );
}

void Tunnel::showHide( bool show )
{
    QList<Tunnel*>* list = m_tunnels.value( m_name );
    if( !list ) return;
    for( Tunnel* tunnel: *list ) tunnel->m_show = show;
    Circuit::self()->update();
}

QRectF Tunnel::boundingRect() const
{
    if( m_packed ) return QRectF( 0, 0, 0 ,0 );
    else return Component::boundingRect();
}

void Tunnel::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    if( !Linker::m_selecComp )  // Used when linking or creating Boards to set this as main component
        Component::mousePressEvent( event ); // Tunnel should not be linked or main component
}

void Tunnel::paint( QPainter* p, const QStyleOptionGraphicsItem *o, QWidget *w )
{
    if( m_hidden || m_packed ) return;

    if( m_tunnels.contains( m_name ) )
    {
        if( m_pin[0]->wireFlags() & wireBus ) m_color = QColor( 100, 220, 100 );
        else                    m_color = QColor( 255, 255, 250 );
    } else                      m_color = QColor( 210, 210, 230 );

    Component::paint( p, o, w );

    /*if( m_rotated ){
        QPointF points[5] =        {
            QPointF( m_size+8,-4 ),
            QPointF(  8,-4 ),
            QPointF(  4, 0 ),
            QPointF(  8, 4 ),
            QPointF( m_size+8, 4 ) };

        p->drawPolygon( points, 5 );
    } else {*/
        QPointF points[5] =        {
            QPointF(-m_size-8,-4 ),
            QPointF(  -8,-4 ),
            QPointF(  -4, 0 ),
            QPointF(  -8, 4 ),
            QPointF(-m_size-8, 4 ) };

        p->drawPolygon( points, 5 );
    //}
    if( m_show ){
        p->setOpacity( 0.4 );
        p->fillRect( boundingRect(), Qt::darkBlue );
        p->setOpacity( 1 );
    }
    Component::paintSelected( p );
}
