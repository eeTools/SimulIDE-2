/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

// Component made of interconnected Functional Blocks

#include <QGraphicsProxyWidget>
#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QMap>

#include "fcomponent.h"
#include "fmodule.h"
#include "canvasbase.h"
//#include "module.h"
#include "m_pinport.h"
#include "iopin.h"
#include "mainwindow.h"
#include "componentlist.h"
#include "blocklist.h"
#include "propdialog.h"
#include "utils.h"

//#include "intfprop.h"

#define tr(str) simulideTr("fComponent",str)

QStringList fComponent::m_shapes = {"None","Rectangle","Triangle","Diamond","Ellipse","And","Or","Xor"};
QString fComponent::getShapes() { return m_shapes.join(",")+";"
                                 +tr("None")+","+tr("Rectangle")+","+tr("Triangle")+","+tr("Diamond")+","+tr("Ellipse")+","
                                 +tr("And")+","+tr("Or")+","+tr("Xor");}

fComponent::fComponent( QString type, int id, QGraphicsScene* canvas )
          : Component( id )
          , Element()
{
    m_canvas = canvas;
    m_type   = type;
    m_width  = 4;
    m_height = 4;
    m_minWidth  = 4;
    m_minHeight = 4;
    m_startHalf = false;
    m_compChanged = false;
    setShapeStr("Rectangle");

    m_area = QRect(-m_width*8/2,-m_height*8/2, m_width*8, m_height*8 );

    /*addPropGroup( { tr("Package"),
    {
        new BoolProp("starthalf", tr("Start at half Cell"), this, { nullptr,"", P_BOOL  , 0, 0 } ),
        new IntProp("width"     , tr("Width")             , this, { nullptr,"", P_INT   , 0, 0 } ),
        new IntProp("height"    , tr("Height")            , this, { nullptr,"", P_INT   , 0, 0 } ),
        new StrProp("shape"     , tr("Shape")             , this, { nullptr,"", P_ENUM  , 0, 0 } ),
        new StrProp("color"     , tr("Color")             , this, { nullptr,"", P_STRING, 0, 0 } ),
        new StrProp("background", tr("Background image")  , this, { nullptr,"", P_STRING, 0, 0 } ),
        new StrProp("invertedpins",""                     , this, { nullptr,"", P_STRING, propHidden, 0 } ),
    }, groupPkg } );*/
}
fComponent::~fComponent(){}

void fComponent::initialize()
{
    for( fModule* module : m_modules.values() ) module->initModule();
}

void fComponent::voltChanged()
{
    while( m_compChanged )
    {
        m_compChanged = false;
        for( fModule* module : m_activeMod )  /// TODO: optimize list
            module->runStep();
    }
}

void fComponent::setup() // Called from Circuit
{
    /*struct prop_t{
        QString name;
        QString value;
    };*/

    QString filePath = ComponentList::self()->getDataFile( m_type );
    if( filePath.isEmpty() )
    {
        qDebug() << "fComponent::setup: no file for Component " << m_type;
        return;
    }

    QMap<QString, ComProperty*> myProperties;

    QString doc = fileToString( filePath, "fComponent::setup");
    QStringList docLines = doc.split("\n");
    docLines.removeAll("");

    //Module* module;

    for( QString line : docLines )
    {
        QStringList tokens = line.split("; ");
        QString type = tokens.takeFirst();

        QList<prop_t> properties;
        for( QString prop : tokens )
        {
            QStringList p = prop.split("=");
            if( p.size() != 2 ) continue;
            properties.append( {p.first(), p.last()} ); // Property_name, Property_value
        }
        if( properties.isEmpty() ){
            qDebug() << "fComponent::setup Component document Error";
            break;
        }

        QString uid = properties.takeFirst().value;
        //QString newUid = uid;

        if( type == "FuncWire" )
        {
            QString signalStr;
            fModule* signalModule = nullptr;
            fModule* slotModule   = nullptr;
            //ModSlot* slot = nullptr;

            for( prop_t prop : properties )
            {
                if( prop.name == "pin0" )
                {
                    QStringList words = prop.value.split("@");
                    QString modUid = words.takeLast() ;
                    signalModule = m_modules.value( modUid );
                    signalStr = words.join("@").remove("Signal");
 qDebug() << "fComponent::setup  signalStr " << signalStr ;
                }
                else if( prop.name == "pin1" )
                {
                    QStringList words = prop.value.split("@");
                    QString modUid = words.takeLast() ;
                    slotModule = m_modules.value( modUid );
                    QString slotStr = words.join("@").remove("Slot");
                    //slot = slotModule->getSlot( slotStr );
 qDebug() << "fComponent::setup  slotStr " << slotStr ;
                    if( signalStr.isEmpty() )
                    {
                        qDebug() << "fComponent::setup Error creating Connection " << uid;
                    }
                    else if( slotModule) // Create connetion Signal->Slot
                    {
                        int slotId = slotStr.toInt();
                        slotModule->connectSignal( signalModule, slotId );
                        //signalModule->connect( signalStr, slot, &slotModule->m_modChanged, &m_compChanged );
                    }
                    else           // Must be a property
                    {
                        /*
                        ComProperty* signalPrp = myProperties.value( signalStr );
                        ComProperty* slotPrp   = slotModule->getProperty( slotStr );
                        if( signalPrp && slotPrp ) signalPrp->addCallBack( slotPrp );
                        else                       qDebug() << "fComponent::setup Error creating Property Connection " << uid << signalStr << slotStr;
                        */
                    }
                }
            }
        }
        else if( type == "Component" )
        {
            for( prop_t prop : properties )
                this->setPropStr( prop.name, prop.value );
        }
        else if( type == "Property" )
        {
            QString id;
            QString name;
            QString valStr;

            for( prop_t prop : properties )
            {
                if( prop.name == "name"   ) { name = prop.value; id = name.toLower(); }
                if( prop.name == "propval") valStr = prop.value;
            }
            ///ComProperty* prop = new IntPropF( id, name, this, { nullptr,"V", P_INT, 0, 0 } );
            ///prop->setValStr( valStr );
            ///myProperties.insert( id, prop );
        }
        else   // Create Module
        {
            fModule* fmodule = BlockList::self()->createModule( type, uid, this, properties );
            ///
            //module = (Module*)BlockList::self()->createItem( type, uid );
            /*if( module ){
                module->setComponent( this );
                m_modules.insert( uid, fmodule );
                m_activeMod.append( fmodule );

                for( prop_t prop : properties )
                    module->setPropStr( prop.name, prop.value );
            }
            else qDebug() << "fComponent::setup Error creating Module" << uid;*/
        }
    }

    if( myProperties.size() )
    {
        ///this->addPropGroup( { "Main", {}, 0 } );
        ///for( ComProperty* prop : myProperties.values() ) addProperty( "Main", prop );
    }
}

void fComponent::addPort( PortModule* port )
{
    if( !m_ports.contains( port ) ) m_ports.append( port );
    upDateShape();
}

QGraphicsProxyWidget* fComponent::addWidget( QWidget* widget )
{
    QGraphicsProxyWidget* proxy = m_canvas->addWidget( widget );
    proxy->setParentItem( this );
    return proxy;
}

void fComponent::setWidth( int width )
{
    if( width < 1 ) width = 1;
    if( m_minWidth == width ) return;

    m_minWidth = width;
    upDateShape();
}

void fComponent::setHeight( int height )
{
    if( height < 1 ) height = 1;
    if( m_minHeight == height ) return;

    m_minHeight = height;
    upDateShape();
}

void fComponent::upDateShape()
{
    QList<PortModule*> rigPorts;
    QList<PortModule*> lefPorts;
    QList<PortModule*> topPorts;
    QList<PortModule*> botPorts;

    double start = m_startHalf ? 0.5 : 1;

    double rSize=start, lSize=start, tSize=start, bSize=start;

    for( PortModule* port : m_ports )
    {
        int size = port->size();

        switch( port->sideTipe() )
        {
            case Right:{
                rigPorts.append( port );
                rSize += port->position();
                rSize += size;
            }break;
            case Left:{
                lefPorts.append( port );
                lSize += port->position();
                lSize += size;
            }break;
            case Top:{
                topPorts.append( port );
                tSize += port->position();
                tSize += size;
            }break;
            case Bottom:{
                botPorts.append( port );
                bSize += port->position();
                bSize += size;
            }break;
        }
    }
    int pHeight = rSize > lSize ? rSize : lSize;
    int pWidth  = tSize > bSize ? tSize : bSize;
    m_height = pHeight > m_minHeight ? pHeight : m_minHeight;
    m_width  = pWidth  > m_minWidth  ? pWidth  : m_minWidth;

    if( m_width  < 2 ) m_width  = 2;
    if( m_height < 2 ) m_height = 2;

    int width  = m_width*8;
    int height = m_height*8;
    m_area = QRect( -width/2, -height/2, width, height );

    double pos = start;
    for( PortModule* port : rigPorts )
    {
        int portPos = port->position();
        if( portPos == -1 ) pos = (double)m_height/2;
        else                pos += portPos;
        port->updatePosition( pos );
        pos += port->size();
    }
    pos = start;
    for( PortModule* port : lefPorts )
    {
        pos += port->position();
        port->updatePosition( pos );
        pos += port->size();
    }
    pos = start;
    for( PortModule* port : topPorts )
    {
        pos += port->position();
        port->updatePosition( pos );
        pos += port->size();
    }
    pos = start;
    for( PortModule* port : botPorts )
    {
        pos += port->position();
        port->updatePosition( pos );
        pos += port->size();
    }
    m_canvas->update();
}

void fComponent::setShapeStr( QString shape )
{
    if( !m_shapes.contains(shape) ) shape = "Rectangle";
    m_shapeStr = shape;
    m_shape = (fShape_t)m_shapes.indexOf(shape);
    update();
}

void fComponent::setBackground( QString bck )
{
    m_background = bck;

    if( bck.isEmpty() ) return;

    /// TODO define path for these components
    QString  pixmapPath = MainWindow::self()->getDataFilePath("images/"+bck );
    if( QFile::exists( pixmapPath ) )
    {
        if( m_backPixmap ) delete m_backPixmap;
        m_backPixmap = new QPixmap( m_width*32, m_height*32 );
        //m_bckImage = m_bckImage.scaled( m_width*32, m_height*32 );
        //QSvgRenderer* ren = new QSvgRenderer( QString("/home/user/andgate.svg") );
        //QPainter painter( m_backPixmap );
        //ren->render(&painter);
        m_backPixmap->load( pixmapPath );
    }

    update();
}

void fComponent::updatePins()
{
    m_pin.clear();

    for( PortModule* port : m_ports )
    {
        if( port->portType() != PortModule::portIO ) continue;

        mPinPort* ioPort = static_cast<mPinPort*>(port);
        for( IoPin* pin : ioPort->getIoPins() )
        {
            m_pin << pin;
            //if( m_invertedPins.contains( pin->pinId() ) )
            //    pin->setInverted( true );
        }
    }
}

QString fComponent::invertedPins()
{
    QString invPins;
    for( Pin* pin : m_pin )
        if( pin->inverted() ) invPins += pin->pinId()+",";

    return invPins;
}

void fComponent::setInvertedPins( QString pins )
{
    m_invertedPins = pins;
}

void fComponent::remove()
{
    for( fModule* module : m_modules )
    {
        module->remove();
        delete module;
    }
}

void fComponent::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    int endY = m_area.height()/2;
    int endX = m_area.width()/2;

    switch( m_shape )
    {
        case shapeNone: break;
        case shapeRect: p->drawRoundedRect( m_area, 1, 1); break;
        case shapeDiam:
        {
            QPainterPath path;
            QVector<QPointF> points;
            points << QPointF(    0,-endY )
                   << QPointF( endX, 0    )
                   << QPointF(    0, endY )
                   << QPointF(-endX, 0    );

            path.addPolygon( QPolygonF(points) );
            path.closeSubpath();
            p->drawPath( path );
        } break;
        case shapeElli: p->drawEllipse( m_area ); break;
        case shapeTria:
        {
            QPainterPath path;
            path.moveTo(-endX,-endY );
            path.lineTo(-endX, endY );
            path.lineTo( endX, 0    );
            path.lineTo(-endX,-endY );
            p->drawPath( path );
        } break;
        case shapeAnd:
        {
            endX += 1;
            QPainterPath path;
            path.moveTo(-endX,-endY );
            path.quadTo( QPoint( endX,-endY ), QPoint( endX, 0    ) );
            path.quadTo( QPoint( endX, endY ), QPoint(-endX, endY ) );
            path.lineTo(-endX, -endY );
            p->drawPath( path );
        } break;
        case shapeOr:
        {
            endX += 1;
            QPainterPath path;
            path.moveTo(-endX-1,-endY );
            path.quadTo( QPoint( endX-2,-endY ), QPoint( endX   , 0    ) );
            path.quadTo( QPoint( endX-2, endY ), QPoint(-endX-1 , endY ) );
            path.quadTo( QPoint(-endX+3  , 0  ), QPoint(-endX-1 ,-endY ) );
            p->drawPath( path );
        } break;
        case shapeXor:
        {
            endX += 1;
            QPainterPath path;
            path.moveTo(-endX+2,-endY );
            path.quadTo( QPoint( endX-2,-endY ), QPoint( endX   , 0    ) );
            path.quadTo( QPoint( endX-2, endY ), QPoint(-endX+2 , endY ) );
            path.quadTo( QPoint(-endX+6, 0    ), QPoint(-endX+2 ,-endY ) );
            p->drawPath( path );

            p->setBrush( Qt::NoBrush );

            QPainterPath curve;
            curve.moveTo(-endX-1, endY );
            curve.quadTo( QPointF(-(double)endX+4.5, 0 ), QPoint(-endX-1,-endY ) );
            p->drawPath( curve );
        } break;
    }

    //if( true ) p->drawImage( m_area, m_bckImage );
    if( m_backPixmap ) p->drawPixmap( QRect(m_area.x(), m_area.y(), m_width*8, m_height*8), *m_backPixmap );
    else{
        /*if( m_backData  )
        {
            int w = m_backData->size();
            int h = m_backData->at(0).size();
            int mW = m_area.x()+(m_width*8 - w)/2;
            int mH = m_area.y()+(m_height*8 - h)/2;

            for( int x=0; x<w; x++ )
                for( int y=0; y<h; y++ )
                    p->fillRect( QRectF( mW+x, mH+y, 1, 1), QColor(m_backData->at(x).at(y) ) );
        }*/
    }
    Component::paintSelected( p );
}
