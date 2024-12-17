/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PINBASE_H
#define PINBASE_H

#include <QGraphicsItem>

#include "updatable.h"

class Route;
class eNode;

class PinBase : public QGraphicsItem, public Updatable
{
    public:
        PinBase( int angle, QPoint pos, QString id, QGraphicsItem* parent=0, int length=8 );
        ~PinBase();

        enum pinType_t{
            pinNormal=0,
            pinSocket,
            pinHeader,
            pinNull,
            pinRst,
            pinHook
        };

        QRectF boundingRect() const override { return m_area; }
        void setBoundingRect( QRect area ) { m_area = area; }

        virtual QString pinId() { return ""; }
        
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

        virtual Route* wire() { return m_wire; }
        void setWire( Route* c );
        virtual void removeWire();

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

        virtual void setPinId( QString id ) {;}
        void setVisible( bool visible );

        virtual QColor getColor(){ return m_color[0]; }

        void moveBy( int dx, int dy );

        virtual void wireRemoved();

        void setPinType( pinType_t ty ) { m_pinType = ty; }
        pinType_t pinType() { return m_pinType; }

        void warning( bool w );
        virtual void animate( bool an );

        virtual void isMoved();
        void flip( int h, int v );

    protected:
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

        QString m_labelText;
        
        QColor m_color[8];
        QRectF m_area;

        Route* m_wire;

        PinBase* m_conPin;  // Pin at the other side of wire

        QGraphicsSimpleTextItem m_label;
};

#endif
