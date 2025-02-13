/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "inductance.h"

class Inductor : public Component, public Inductance
{
    public:
        Inductor( int id );
        ~Inductor();

 static listItem_t registerItem();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:

};
