/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "element.h"

#include "doubprop.h"
#include "boolprop.h"

class IoPin;

class Probe : public Component, public Element
{
    public:
        Probe( int id );
        ~Probe();

        virtual void initialize() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;

        void setVolt( double volt );

        //bool isSmall() { return m_small; }
        void setSmall( bool s );

        //double threshold() { return m_voltTrig; }
        //void setThreshold( double t ) { m_voltTrig = t; }

        //bool pauseState() { return m_pauseState; }
        //void setPauseState( bool s ) { m_pauseState = s; }

        void rotateAngle( double a ) override;

        void propertyChanged( const ComProperty* prop ) override;

        QPainterPath shape() const override;
        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

 static listItem_t registerItem();

    protected:
        void contextMenu( QGraphicsSceneContextMenuEvent* e, QMenu* m ) override;
        void slotBreakpoint();

    private: 
        double m_voltIn;
        //double m_voltTrig;

        /*bool m_showVolt;
        bool m_small;
        bool m_pause;*/
        bool m_state;

        QString m_voltStr;

        IoPin* m_inputPin;

        // Properties -------------------------------------------------

        DoubProp m_threshold;
        BoolProp m_showVolt;
        BoolProp m_small;
        BoolProp m_pause;
};
