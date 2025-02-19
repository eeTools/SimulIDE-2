/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "pn.h"
#include "component.h"

class Diode : public Component, public PN
{
    public:
        Diode( int id, bool zener=false );
        ~Diode();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

 static listItem_t registerItem();
    private:
};
