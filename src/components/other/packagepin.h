/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PACKAGEPIN_H
#define PACKAGEPIN_H

#include "pin.h"

class SubPackage;
class PackagePin : public Pin
{
    public:
        PackagePin( int angle, const QPoint pos, QString id, SubPackage* parent=0 );
        ~PackagePin();

        void setLabelText( QString label, bool over=true ) override;
        void setLength( int length ) override;

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseMoveEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) override;
        void contextMenuEvent( QGraphicsSceneContextMenuEvent* event ) override;

    private:
        SubPackage* m_package;
};
#endif
