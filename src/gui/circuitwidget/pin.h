/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PIN_H
#define PIN_H

#include "pinbase.h"
#include "e-pin.h"

enum pinState_t{
    undef_state=0,
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

class Pin : public PinBase, public ePin
{
    Q_INTERFACES(QGraphicsItem)

    public:
        Pin( int angle, QPoint pos, QString id, int index, Component* parent=0, int length=8 );
        ~Pin();

        enum { Type = UserType + 3 };
        int type() const override { return Type; }

        void updateStep() override;

        QString pinId() override { return m_id; }

        void setColor( QColor color ) { m_color[0] = color; }

        Component* component() { return m_component; }

        void setConPin( Pin* pin ) { m_conPin = pin; }
        Pin* conPin(){ return m_conPin; }

        Pin* connectPin( bool connect );

        void setPinId( QString id ) override { m_id = id; }

        void removeWire() override;
        void wireRemoved() override;

        void registerEnode( eNode* enode, int n=-1 );
        void registerPinsW( eNode* enode, int n=-1 );
        
        void setIsBus( bool bus );
        bool isBus() { return m_isBus; }

        void setDataChannel( LaChannel* ch ) { m_dataChannel = ch; }

        void animate( bool an ) override;
        void isMoved() override;

        virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;

        pinState_t m_pinState;

        bool m_isBus;

        Component* m_component;
        LaChannel* m_dataChannel;    // connect to Logic Analyzer

        Pin* m_conPin;         // Pin at the other side of wire
};

#endif
