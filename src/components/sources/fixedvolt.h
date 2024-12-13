/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef FIXEDVOLT_H
#define FIXEDVOLT_H

#include <QToolButton>
#include <QGraphicsProxyWidget>

#include "component.h"
#include "pinsource.h"

class CustomButton;

class FixedVolt : public Component
{
    public:
        FixedVolt( QString id );
        ~FixedVolt();

 static listItem_t libraryItem();

        void initialize() override;
        void updateStep() override;

        bool out();
        virtual void setOut( bool out );

        double voltage() { return m_voltage; }
        void setVoltage( double v );

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

        virtual void onbuttonclicked();

    protected:
        //void updateOutput();

        double m_voltage;

        Pin* m_outpin;
        PinSource m_out;

        CustomButton* m_button;
        QGraphicsProxyWidget* m_proxy;
};

#endif
