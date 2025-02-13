/***************************************************************************
 *   Copyright (C) 202 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"

class Pin;

class Tunnel : public Component
{
    public:
        Tunnel( int id );
        ~Tunnel();

 static listItem_t registerItem();

        QRectF boundingRect() const override;

        QString name() { return m_name; }
        void setName( QString name );

        bool isBus();
        void setIsbus( bool b );

        bool rotated() { return m_rotated; }
        void setRotated( bool rot );

        void remove() override;

        void registerEnode( int enode, int n=-1 ) override;

        QString tunnelUid() { return m_tunUid; }
        void setTunnelUid( QString uid ) { m_tunUid = uid; }

        void setEnode( int node, int n=-1 );
        void setPacked( bool p );
        void removeTunnel();
        bool m_show;

        Pin* getPin() { return m_pin[0]; }
 static int getEnode( QString n );

 static void clearTunnels();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    public slots:
        void showGroup();
        void hideGroup() { showHide( false ); }
        void renameGroup();

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;
        void contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu ) override;

        void showHide( bool show );
        void setGroupName( QString name, bool single );

        int m_size;

        QString m_name;
        QString m_tunUid;

        bool m_rotated;
        bool m_blocked;
        bool m_packed;

 static QHash<QString, QList<Tunnel*>*> m_tunnels;
};
