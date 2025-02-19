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

        void setValue( const uint8_t idInt, const value_t &val ) override;

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

        uint8_t m_resistProp;
        double  m_resistVal;

        Resistance m_resistance;
};
