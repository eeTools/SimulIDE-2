/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef FCOMPONENT_H
#define FCOMPONENT_H

#include "component.h"
#include "element.h"

class Module;
class PortModule;
class CanvasBase;
class QGraphicsProxyWidget;

enum fShape_t{
    shapeNone=0,
    shapeRect,
    shapeTria,
    shapeDiam,
    shapeElli,
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

        QString shapeStr() { return m_shapeStr;}
        void setShapeStr( QString shape );

        void setBackground( QString bck ) override;

        QString invertedPins();
        void setInvertedPins( QString pins );

        void addPort( PortModule* port );
        /// TODO void removePort( PortModule* port );
        QGraphicsProxyWidget* addWidget( QWidget* widget );

        void upDateShape();
        void updatePins();

        void remove() override;

 static QString getShapes();

    protected:
        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    private:

        int m_width;
        int m_height;
        int m_minWidth;
        int m_minHeight;

        bool m_startHalf;
        bool m_compChanged;

        QString m_invertedPins;

        fShape_t m_shape;
        QString m_shapeStr;
 static QStringList m_shapes;

        QMap<QString, Module*> m_modules;
        QList<Module*> m_activeMod;

        QList<PortModule*> m_ports;

        QGraphicsScene* m_canvas;
};
#endif
