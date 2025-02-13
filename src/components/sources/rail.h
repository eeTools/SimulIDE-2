/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "pinsource.h"

class IoPin;

class Rail : public Component
{
    public:
        Rail( int id );
        ~Rail();

 static listItem_t registerItem();

        void initialize() override;
        void updateStep() override;

        double voltage() { return m_voltage; }
        void setVoltage( double v );

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    private:
        double m_voltage;

        IoPin* m_out;
};
