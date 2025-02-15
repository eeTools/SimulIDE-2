/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef HOOK_H
#define HOOK_H

#include "pinbase.h"
#include "fblock.h"
#include "hooktype.h"

class Connector;

class Hook : public PinBase
{
    Q_INTERFACES(QGraphicsItem)

    public:
        Hook( int angle, QPoint pos, QString id, hookType_t type, QGraphicsItem* parent=0 );
        ~Hook();

        int type() const override { return UserType + 4; }

        QColor getColor() override { return m_color[m_hookType]; }

        WireBase* wire() override { return nullptr; } // We can have more than 1 wire.
        void setWire( WireBase* c ) override;
        void wireRemoved( WireBase* w ) override;
        void removeWire() override;

        void isMoved() override;

        void paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* ) override;

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;

        hookType_t m_hookType;

        bool m_output;

        QList<WireBase*> m_wireList;
};

#endif
