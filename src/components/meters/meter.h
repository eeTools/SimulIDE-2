/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "resistance.h"
#include "component.h"

class IoPin;

class Meter : public Component, public Resistance
{
    public:
        Meter( int id );
        ~Meter();

        bool swithchPins() { return m_switchPins; }
        void setSwitchPins( bool s );

        /// void initialize() override { m_crashed = false;}
        void updateStep() override;

        void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;

    protected:
        void setflip() override;

        QString m_unit;
        double m_dispValue;
        bool m_switchPins;

        //IoPin* m_outPin;
        QGraphicsSimpleTextItem m_display;
};
