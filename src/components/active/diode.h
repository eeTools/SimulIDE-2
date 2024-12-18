/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef DIODE_H
#define DIODE_H

#include "pn.h"
#include "component.h"

class Diode : public Component, public PN
{
    public:
        Diode( QString id, bool zener=false );
        ~Diode();

 static listItem_t registerItem();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    private:
};

#endif
