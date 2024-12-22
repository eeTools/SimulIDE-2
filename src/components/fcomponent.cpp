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
#include "canvasbase.h"
#include "module.h"
#include "m_ioport.h"
#include "mainwindow.h"
#include "componentlist.h"
#include "blocklist.h"
#include "propdialog.h"
#include "utils.h"

#include "intfprop.h"
#include "intprop.h"
#include "boolprop.h"
#include "stringprop.h"

#define tr(str) simulideTr("fComponent",str)

QStringList fComponent::m_shapes = {"None","Rectangle","Diamond","Ellipse","And","Or","Xor"};
QString fComponent::getShapes() { return m_shapes.join(",")+";"
                                 +tr("None")+","+tr("Rectangle")+","+tr("Diamond")+","+tr("Ellipse")+","
                                 +tr("And")+","+tr("Or")+","+tr("Xor");}

fComponent::fComponent( QString type, QString id, QGraphicsScene* canvas )
          : Component( id )
          , Element( id )
{
    m_canvas = canvas;
    m_type   = type;
    m_width  = 4;
    m_height = 4;
    m_minWidth = 4;
    m_minHeight = 4;
    m_startHalf = false;
    m_compChanged = false;
    setShapeStr("Rectangle");

    m_area = QRect(-m_width*8/2,-m_height*8/2, m_width*8, m_height*8 );

    addPropGroup( { tr("Package"),
    {
        new BoolProp<fComponent>("starthalf", tr("Start at half Cell"),""
                       , this, &fComponent::startHalf, &fComponent::setStartHalf ),

        new IntProp<fComponent>("width", tr("Width"), ""
                       , this, &fComponent::width, &fComponent::setWidth, 0 ),

        new IntProp<fComponent>("height", tr("Height"), ""
                       , this, &fComponent::height, &fComponent::setHeight, 0 ),

        new StrProp<fComponent>("shape", tr("Shape"), fComponent::getShapes()
                       , this, &fComponent::shapeStr, &fComponent::setShapeStr, 0, "enum" ),

        new StrProp<fComponent>("background", tr("Background"), ""
                       , this, &fComponent::background, &fComponent::setBackground, 0 ),
    },0} );
}
fComponent::~fComponent()
{
    for( Module* m : m_modules ) delete m;
}

void fComponent::initialize()
{
    for( Module* module : m_modules.values() ) module->initModule();
}

void fComponent::voltChanged()
{
    while( m_compChanged )
    {
        m_compChanged = false;
        for( Module* module : m_activeMod )  /// TODO: optimize list
            module->runStep();
    }
}

void fComponent::setup() // Called from Circuit
{
    struct prop_t{
        QString name;
        QString value;
    };

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

    Module* module;

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
        QString newUid = uid;

        if( type == "FuncWire" )
        {
            QString signalStr;
            Module* signalModule = nullptr;
            Module* slotModule   = nullptr;
            ModSlot* slot = nullptr;

            for( prop_t prop : properties )
            {
                if( prop.name == "pin0" )
                {
                    QStringList words = prop.value.split("@");
                    signalModule = m_modules.value( words.takeLast() );
                    signalStr = words.join("@");
                }
                else if( prop.name == "pin1" )
                {
                    QStringList words = prop.value.split("@");
                    slotModule = m_modules.value( words.takeLast() );
                    QString slotStr = words.join("@");
                    slot = slotModule->getSlot( slotStr );

                    if( signalStr.isEmpty() ) qDebug() << "fComponent::setup Error creating Connection " << uid;
                    else if( slot ) // Create connetion Signal->Slot
                    {
                        signalModule->connect( signalStr, slot, &slotModule->m_modChanged, &m_compChanged );
                    }
                    else           // Must be a property
                    {
                        ComProperty* signalPrp = myProperties.value( signalStr );
                        ComProperty* slotPrp   = slotModule->getProperty( slotStr );
                        if( signalPrp && slotPrp ) signalPrp->addCallBack( slotPrp );
                        else qDebug() << "fComponent::setup Error creating Connection " << uid << signalStr << slotStr;
                    }
                }
            }
        }
        else if( type == "Fblock" ) /// Maybe not needed anymore??
        {
            if( !module ) continue;
            prop_t prop = properties.first();
            module->setIndex( prop.value.toInt() );
        }
        else if( type == "Component" )
        {
            continue;
        }
        else if( type == "Property" )
        {
            QString id;
            QString name;
            QString valStr;

            for( prop_t prop : properties )
            {
                if( prop.name == "name" ) { name = prop.value; id = name.toLower(); }
                if( prop.name == "value") valStr = prop.value;
            }
            ComProperty* prop = new IntPropF( id, name, "", this, 0 );
            prop->setValStr( valStr );
            myProperties[id] = prop;
        }
        else   // Create Module
        {
            module = (Module*)BlockList::self()->createItem( type, uid );
            if( module ){
                module->setComponent( this );
                m_modules.insert( uid, module );
                m_activeMod.append( module );

                for( prop_t prop : properties )
                    module->setPropStr( prop.name, prop.value );
            }
            else
                qDebug() << "fComponent::setup Error creating Module" << uid;
        }
    }

    if( myProperties.size() )
    {
        this->addPropGroup( { "Main", {}, 0 } );
        for( ComProperty* prop : myProperties.values() ) addProperty( "Main", prop );
    }
}

void fComponent::addPort( PortBase* port )
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
    QList<PortBase*> rigPorts;
    QList<PortBase*> lefPorts;
    QList<PortBase*> topPorts;
    QList<PortBase*> botPorts;

    double start = m_startHalf ? 0.5 : 1;

    double rSize=start, lSize=start, tSize=start, bSize=start;

    for( PortBase* port : m_ports )
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
    for( PortBase* port : rigPorts )
    {
        int portPos = port->position();
        if( portPos == -1 ) pos = (double)m_height/2;
        else                pos += portPos;
        port->updatePosition( pos );
        pos += port->size();
    }
    pos = start;
    for( PortBase* port : lefPorts )
    {
        pos += port->position();
        port->updatePosition( pos );
        pos += port->size();
    }
    pos = start;
    for( PortBase* port : topPorts )
    {
        pos += port->position();
        port->updatePosition( pos );
        pos += port->size();
    }
    pos = start;
    for( PortBase* port : botPorts )
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

    if( bck.startsWith("color") )
    {
        bck.remove("color").remove("(").remove(")").remove(" ");
        QStringList rgb = bck.split(",");
        if( rgb.size() < 3 ) return;

        m_color = QColor( rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt() );
    }
    else if( bck != "" )
    {
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
    }
    update();
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
        case shapeAnd:
        {
            endX += 1;
            QPainterPath path;
            path.moveTo(-endX,-endY );
            path.quadTo( QPoint( endX,-endY ), QPoint( endX, 0 ) );
            path.quadTo( QPoint( endX, endY ), QPoint(-endX, endY ) );
            path.lineTo( -endX, -endY );
            p->drawPath( path );
        } break;
        case shapeOr:
        {
            endX += 1;
            QPainterPath path;
            path.moveTo(-endX-1,-endY );
            path.quadTo( QPoint( endX-2,-endY ), QPoint( endX, 0    ) );
            path.quadTo( QPoint( endX-2, endY ), QPoint(-endX-1 , endY ) );
            path.quadTo( QPoint(-endX+3  , 0  ), QPoint(-endX-1 ,-endY ) );
            p->drawPath( path );
        } break;
        case shapeXor:
        {
            endX += 1;
            QPainterPath path;
            path.moveTo(-endX+2,-endY );
            path.quadTo( QPoint( endX-2,-endY ), QPoint( endX, 0   ) );
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
    else
    {
        //p->drawRoundedRect( m_area, 1, 1);
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
