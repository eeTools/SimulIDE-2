/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef INDUCTOR_H
#define INDUCTOR_H

#include "component.h"
#include "inductance.h"

class Inductor : public Component, public Inductance
{
    public:
        Inductor( QString id );
        ~Inductor();

 static listItem_t registerItem();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:

};

#endif
