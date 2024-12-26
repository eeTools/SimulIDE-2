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
#include "funcwire.h"
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
    for( WireBase*  wire  : m_wireList  ) component += wire->toString();
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
    fComponent* fComp = ComposerWidget::self()->package();

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

        if( type == "FuncWire" )
        {
            WireBase* wire = createWire( properties, newUid );
            if( wire ) m_wireList.append( wire );
        }
        else if( type == "Package" || type == "Component" )
        {
            for( prop_t prop : properties )
                fComp->setPropStr( prop.name, prop.value );
        }
        else   // Create Module
        {
            FuncBlock* block = createBlock( type, uid );
            if( !block ) continue;
            for( prop_t prop : properties )
                block->setPropStr( prop.name, prop.value );
        }
    }
}

FuncBlock* Composer::createBlock( QString type, QString id )
{
    fComponent* fComp = ComposerWidget::self()->package();
    if( id.isEmpty() ) id = newSceneId();
    FuncBlock* fb = new FuncBlock( fComp, type, id );
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

WireBase* Composer::newWire( QString id, PinBase* startPin, PinBase* endPin )
{
    return new FuncWire( id, startPin, endPin );
}
