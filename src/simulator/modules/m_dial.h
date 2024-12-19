/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef MDIAL_H
#define MDIAL_H

#include "module.h"
#include "modsignal.h"
#include "dialwidget.h"

class mDial : public Module
{
    public:
        mDial( QString name );
        ~mDial();

 static listItem_t registerItem();

        void setComponent( fComponent* c ) override;
        void initModule() override;
        void runStep() override;

        int maxVal()  { return m_maxVal; }
        void setMaxVal( int max );

        int minVal() { return m_minVal; }
        void setMinVal( int min );

        int value();
        void setValue( int v );

        int steps() { return m_steps; }
        void setSteps( int s );

        double angle() { return m_angle; }
        void setAngle( double a );

        QPointF position() { return m_position; }
        void setPosition( QPointF p);

        bool slider() { return m_slider; }
        void setSlider( bool s );

        double scale() { return m_dialW->scale(); }
        void setScale( double s );

    public slots:
        void dialChanged( int );

    private:
        void updateProxy();

        int m_output;

        ModSignal m_outSignal;

        // Dial ---------------------------

        int m_minVal;
        int m_maxVal;
        int m_steps;

        double m_angle;
        QPoint m_position;

        bool m_slider;

        DialWidget* m_dialW;
        QGraphicsProxyWidget* m_proxy;
};
#endif
