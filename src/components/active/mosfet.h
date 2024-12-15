/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MOSFET_H
#define MOSFET_H

#include "fet.h"
#include "component.h"

class Mosfet : public Component, public Fet
{
    public:
        Mosfet( QString id );
        ~Mosfet();

 static listItem_t libraryItem();

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w );
};

#endif
