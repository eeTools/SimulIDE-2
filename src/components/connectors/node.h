/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "pin.h"

class Node : public Component
{
    public:
        QRectF boundingRect() const override { return QRect(-4,-4, 8, 8 ); }

        Node(int id );
        ~Node();

        Pin* getPin( int pin ) const { return m_pin[pin]; }

        void registerEnode( int enode, int n=-1 ) override;

        void setHidden( bool hid, bool hidArea=false, bool hidLabel=false ) override;

        void pinMessage( int rem=1 ) override;
        bool checkRemove();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        void contextMenuEvent( QGraphicsSceneContextMenuEvent* ) override {;}

    private:
        void joinConns( int co0, int c1);

        bool m_isBus;
        bool m_blocked;
};
