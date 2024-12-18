/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef RAIL_H
#define RAIL_H

#include "component.h"
#include "pinsource.h"

class Rail : public Component
{
    public:
        Rail( QString id );
        ~Rail();

 static listItem_t registerItem();

        virtual void initialize() override;
        virtual void updateStep() override;

        double voltage() { return m_voltage; }
        void setVoltage( double v );

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    private:
        double m_voltage;

        PinSource m_out;
};

#endif
