/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef CSOURCE_H
#define CSOURCE_H

#include "component.h"
#include "resistance.h"

class LibraryItem;

class Csource : public Component, public Resistance
{
    public:
        Csource( QString id );
        ~Csource();

 static listItem_t libraryItem();

        void initialize() override;
        void voltChanged() override;
        void updateStep() override;

        double gain() { return m_gain; }
        void setGain( double g );

        double volt() { return m_voltage; }
        void setVolt( double volt );

        double outCurrent() { return m_current; }
        void setOutCurrent( double c );

        bool currSource() { return m_currSource; }
        void setCurrSource( bool c );

        bool currControl() { return m_currControl; }
        void setCurrControl( bool c );

        bool controlPins() {return m_controlPins; }
        void setControlPins( bool set );

        void setVoltage( double v );

        virtual bool setLinkedTo( Linker* li ) override;
        virtual void setLinkedValue( double v, int i=0 ) override;

        //virtual QPainterPath shape() const;
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected slots:
        virtual void slotProperties() override;

    private:
        void updtProperties();

        double m_voltage;
        double m_current;
        double m_gain;

        double m_lastCurr;

        bool m_controlPins;
        bool m_currSource;
        bool m_currControl;
};

#endif