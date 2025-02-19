/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "element.h"

class IoPin;

class Probe : public Component, public Element
{
    public:
        Probe( int id );
        ~Probe();

 static listItem_t registerItem();

        virtual void initialize() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;

        void setVolt( double volt );

        bool isSmall() { return m_small; }
        void setSmall( bool s );

        double threshold() { return m_voltTrig; }
        void setThreshold( double t ) { m_voltTrig = t; }

        bool pauseState() { return m_pauseState; }
        void setPauseState( bool s ) { m_pauseState = s; }

        void rotateAngle( double a ) override;

        QPainterPath shape() const override;
        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        void contextMenu( QGraphicsSceneContextMenuEvent* e, QMenu* m ) override;
        void slotBreakpoint();

    private: 
        double m_voltIn;
        double m_voltTrig;

        bool m_showVolt;
        bool m_small;
        bool m_pauseState;
        bool m_state;

        QString m_voltStr;

        IoPin* m_inputPin;
};
