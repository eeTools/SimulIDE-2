/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef RESISTOR_H
#define RESISTOR_H

#include "component.h"
#include "resistance.h"

class Resistor : public Component, public Resistance
{
    public:
        Resistor( QString id );
        ~Resistor();

 static listItem_t libraryItem();

        void initialize() override;

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;
};

#endif
