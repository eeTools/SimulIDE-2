/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>

#include "composer.h"
#include "composerwidget.h"
#include "composerview.h"
#include "canvasview.h"
#include "fcomponent.h"
#include "blocklist.h"
#include "canvas.h"
#include "fblock.h"
#include "module.h"
#include "hook.h"
#include "linkage.h"
#include "utils.h"

Composer* Composer::m_pSelf = nullptr;

Composer::Composer( qreal w, qreal h, ComposerView* parent )
        : CanvasBase( w, h, parent )
{
    m_pSelf = this;

    this->setBackgroundBrush( QColor( 200, 200, 200 ) );
}
Composer::~Composer(){}

void Composer::clearCanvas() // Remove everything
{
    if( m_newWire ) deleteNewWire();
    //m_deleting = true;

    for( FuncBlock* block : m_blockList )
    {
        block->remove();
        if( block->scene() ) removeItem( block );
    }
    m_blockList.clear();

    CanvasBase::clearCanvas();

    //m_deleting = false;
}

QString Composer::toString()
{
    QString component;

    for( FuncBlock* block : m_blockList ) component += block->toString();
    for( Route* conn  : m_wireList  ) component += conn->toString();
    component += "\n";

    return component;
}

void Composer::loadComponent( QString path )
{
    if( m_newWire ) deleteNewWire();

    QString doc = fileToString( path, "Circuit::loadCircuit" );

    clearCanvas();
    loadStrDoc( doc );
}

void Composer::loadStrDoc( QString doc )
{
    m_fComp = ComposerWidget::self()->package();

    Module* module = nullptr;
    QStringList docLines = doc.split("\n");
    docLines.removeAll("");

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
        if( properties.isEmpty() )
        {
            qDebug() << "Composer::loadStrDoc Component document Error";
            return;
        }

        QString uid = properties.takeFirst().value;
        QString newUid = uid;

        if( type == "Wire" )
        {
            Route* wire = createWire( properties, newUid );
            if( wire ) m_wireList.append( wire );
            /*Hook* hook0 = nullptr;
            Hook* hook1 = nullptr;
            for( prop_t prop : properties ){
                if     ( prop.name == "hook0" ) hook0 = getHook( prop.value );
                else if( prop.name == "hook1" ) hook1 = getHook( prop.value );
            }
            if( !hook0 || !hook1 )
            {
                qDebug() << "Error: Missing Hook creating Wire " << uid;
            }else{
                /// FIXME
                /// newWire( hook0, uid );
                /// closeWire( hook1 );
            }*/
        }
        else if( type == "Fblock" )
        {
            if( module ){
                FuncBlock* block = createBlock( module, "", uid );

                for( prop_t prop : properties ){
                    if     ( prop.name == "x" ) block->setX( prop.value.toDouble() );
                    else if( prop.name == "y" ) block->setY( prop.value.toDouble() );
                }
                module = nullptr;
            }
            else qDebug() << "Error: Missing Module creating Block " << uid;
        }
        else if( type == "Package" || type == "Component" )
        {
            if( m_fComp ){
                for( prop_t prop : properties )
                    m_fComp->setPropStr( prop.name, prop.value );
            }
        }
        else   // Create Module
        {
            module = BlockList::self()->createModule( type, uid );
            if( module ){
                module->setComponent( m_fComp );
                for( prop_t prop : properties )
                    module->setPropStr( prop.name, prop.value );
            }
            else qDebug() << "Error creating Module" << uid;
        }
    }
}

FuncBlock* Composer::createBlock( Module* module, QString type, QString id )
{
    if( id.isEmpty() ) id = newSceneId();
    FuncBlock* fb = new FuncBlock( m_fComp, module, type, id );
    QGraphicsScene::addItem( fb );
    m_blockList.insert( fb );
    return fb;
}

void Composer::removeBlock( FuncBlock* fb )
{
    QGraphicsScene::removeItem( fb );
    m_blockList.remove( fb );
}

Hook* Composer::getHook( QString id )
{
    QString blockId = id.split("@").takeLast();
    for( FuncBlock* fb : m_blockList )
        if( fb->uid() == blockId ) return fb->getHook( id );
    return nullptr;
}

Route* Composer::newWire( QString id, PinBase* startPin, PinBase* endPin )
{
    return new Linkage( id, startPin, endPin );
}
