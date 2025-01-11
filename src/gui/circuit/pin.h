/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PIN_H
#define PIN_H

#include "pinbase.h"
#include "kcl.h"

enum pinAnim_t{
    no_anim=0,
    driven_low,    // State is open high, but driven low externally
    open_low,
    open_high,
    out_low,
    out_high,
    input_low,
    input_high
};

class Wire;
class Component;
class LaChannel;

class Pin : public PinBase
{
    Q_INTERFACES(QGraphicsItem)

    public:
        Pin( int angle, QPoint pos, QString id, Component* parent=nullptr, int length=8 );
        ~Pin();

        enum { Type = UserType + 3 };
        int type() const override { return Type; }

        void updateStep() override;

        bool inverted() { return m_inverted; }
        virtual void setInverted( bool i ) { m_inverted = i; }

        //void setNode( int n ) { m_node = n; }
        int getNode()  { return m_node; }

        inline double getVoltage()
        {
            if( m_node < 0 ) return m_voltage;
            return Kcl::self()->getVoltage( m_node );
        }

        void setColor( QColor color ) { m_color[0] = color; }

        Component* component() { return m_component; }

        bool isConnected() { return (m_node >= 0 ); }
        Pin* connectPin( bool connect );

        void removeWire() override;
        void wireRemoved( WireBase* w ) override;

        void registerEnode( int enode, int n=-1 ) override;
        void registerPinsW( int enode, int n=-1 ) override;

        void setDataChannel( LaChannel* ch ) { m_dataChannel = ch; }

        void animate( bool an ) override;
        void isMoved() override;

        virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;

        int m_node;

        bool m_inverted;

        double m_voltage;

        pinAnim_t m_pinAnim;

        Component* m_component;
        LaChannel* m_dataChannel;    // connect to Logic Analyzer
};

#endif
