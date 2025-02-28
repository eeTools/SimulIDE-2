/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "resistance.h"

class Resistor : public Component
{
    public:
        Resistor( int id );
        ~Resistor();

 static listItem_t registerItem();

        void initialize() override;

        void setResistance( double r );

        void propertyChanged( const ComProperty* prop ) override;

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    private:
        Resistance m_resistance;
};
