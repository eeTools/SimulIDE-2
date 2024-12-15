/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef FCOMPONENT_H
#define FCOMPONENT_H

#include "component.h"
#include "element.h"

class Module;
class PortBase;
class CanvasBase;

enum fShape_t{
    shapeNone=0,
    shapeRect,
    shapeDiam,
    shapeCirc,
    shapeAnd,
    shapeOr,
    shapeXor
};

class fComponent : public Component, public Element
{
    public:
        fComponent( QString type, QString id, QGraphicsScene* canvas );
        ~fComponent();

        void setup() override;

        void initialize() override;
        void voltChanged() override;

        int value() { return 0; }
        void setValue( int v ){;}

        int width() { return m_width; }
        void setWidth( int width );

        int height() { return m_height; }
        void setHeight( int height );

        bool startHalf() { return m_startHalf; }
        void setStartHalf( bool h ) { m_startHalf = h; upDateShape(); }

        QString shapeStr() { return "";}
        void setShapeStr( QString s ){;}

        void setBackground( QString bck ) override;

        void addPort( PortBase* port );

        void upDateShape();

    protected:
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    private:

        int m_width;
        int m_height;

        bool m_startHalf;

        fShape_t m_shape;

        QMap<QString, Module*> m_modules;
        QList<Module*> m_activeMod;

        QList<PortBase*> m_ports;

        QGraphicsScene* m_canvas;
};
#endif
