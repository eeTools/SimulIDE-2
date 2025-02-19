/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "fet.h"
#include "component.h"

class Mosfet : public Component, public Fet
{
    public:
        Mosfet( int id );
        ~Mosfet();

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w );

 static listItem_t registerItem();
};
