/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "component.h"
#include "capacitance.h"

class Capacitor : public Component, public Capacitance
{
    public:
        Capacitor( QString id );
        ~Capacitor();

 static listItem_t registerItem();

        void initialize() override;

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;
};

#endif
