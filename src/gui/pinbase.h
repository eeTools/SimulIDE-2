/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PINBASE_H
#define PINBASE_H

#include <QGraphicsItem>

#include "updatable.h"

class WireBase;
class eNode;

class PinBase : public QGraphicsItem, public Updatable
{
    public:
        PinBase( int angle, QPoint pos, QGraphicsItem* parent=0, int length=0 );
        ~PinBase();

        enum pinType_t{
            pinNormal=0,
            pinIO,
            pinSocket,
            pinHeader,
            pinNull,
            pinRst,
            pinHook
        };

        QRectF boundingRect() const override { return m_area; }
        void setBoundingRect( QRect area ) { m_area = area; }

        QString pinId() { return m_id; }
        void setPinId( QString id ) { m_id = id; }
        
        bool unused() { return m_unused; }
        void setUnused( bool unused );

        int length() { return m_length; }
        virtual void setLength( int length );

        void setPinAngle( int angle );
        int pinAngle() { return m_angle; }

        void setY( qreal y );

        void writeWireFlag( int flag, bool val );
        void setWireFlags( int flags ) { m_wireFlags = flags; }
        int  wireFlags() { return m_wireFlags; }

        virtual WireBase* wire() { return m_wire; }
        virtual void setWire( WireBase* c );
        virtual void removeWire(){;}
        virtual void wireRemoved( WireBase* w ){;}

        void setConPin( PinBase* pin ) { m_conPin = pin; }
        PinBase* conPin(){ return m_conPin; }

        virtual void registerEnode( int enode, int n=-1 ){;}
        virtual void registerPinsW( int enode, int n=-1 ){;}

        QString getLabelText() { return m_labelText; }
        virtual void setLabelText( QString label, bool over=true );
        void setLabelPos();
        void setLabelColor( QColor color );
        void setFontSize( int size );
        int  labelSizeX() { return m_labelWidth; }

        void setSpace( double s );
        double space() { return m_space; }

        void setVisible( bool visible );

        virtual QColor getColor(){ return m_color[0]; }

        void setPinType( pinType_t ty ) { m_pinType = ty; }
        pinType_t pinType() { return m_pinType; }

        void warning( bool w );
        virtual void animate( bool an );

        void moveBy( int dx, int dy );
        virtual void isMoved();
        void flip( int h, int v );

    protected:
        QString m_id;

        pinType_t  m_pinType;

        int m_angle;
        int m_length;
        int m_Hflip;
        int m_Vflip;
        int m_overScore;
        int m_labelheight;
        int m_labelWidth;
        int m_wireFlags;

        double m_space;

        bool m_blocked;
        bool m_unused;
        bool m_animate;
        bool m_warning;

        double  m_opCount; // Used by blinking Pin

        QColor m_color[8];
        QRectF m_area;

        WireBase* m_wire;

        PinBase* m_conPin;  // Pin at the other side of wire

        QString m_labelText;
        QGraphicsSimpleTextItem m_label;
};

#endif
