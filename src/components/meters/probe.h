/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PROBE_H
#define PROBE_H

#include "component.h"
#include "element.h"

class IoPin;

class Probe : public Component, public Element
{
    public:
        Probe( QString id );
        ~Probe();

 static listItem_t registerItem();

        virtual void updateStep() override;

        void setVolt( double volt );

        void setSmall( bool s );
        bool isSmall() { return m_small; }

        double threshold() { return m_voltTrig; }
        void setThreshold( double t ) { m_voltTrig = t; }

        void rotateAngle( double a ) override;

        QPainterPath shape() const override;
        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    private: 
        double m_voltIn;
        double m_voltTrig;

        bool m_small;

        IoPin* m_inputPin;
};

#endif
