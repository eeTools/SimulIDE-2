/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef FUNCBLOCK_H
#define FUNCBLOCK_H

#include <QGraphicsItem>
#include <QFont>

#include "compbase.h"

class fComponent;
class PropWidget;
class Module;
class Hook;

class FuncBlock : public QGraphicsItem
{
    Q_INTERFACES( QGraphicsItem )

    public:
        FuncBlock( fComponent* fComp, Module* module, QString type="", QString id="" );
        ~FuncBlock();

        enum { Type = UserType + 10 };
        int type() const override { return Type; }

        QRectF boundingRect() const override { return QRectF( m_area.x()-2, m_area.y()-2, m_area.width()+4, m_area.height()+4 ); }

        void remove();

        void move( QPointF delta );

        void updateWidget();

        QString toString();

        QString uid() { return m_id; }

        Module* module() { return m_module; }

        Hook* getHook( QString id );
        Hook* getSignalHook( QString id );
        Hook* getSlotHook( QString id );

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* ) override;

    protected slots:
        virtual void slotProperties();
        void showToggled( QString propId, bool checked );

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseMoveEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event ) override;

        void setup();

        void updateSignals();
        void updateSlots();
        void updateSize();

        bool m_moving;

        QString m_type;
        QString m_id;

        QRectF  m_area;
        QRectF  m_bodyArea;
        QRectF  m_headArea;
        QRectF  m_PropArea;

        QColor  m_color;
        QColor  m_hColor;
        QColor  m_pColor;

        QFont m_font;

        QList<FuncBlock*> m_blockMoveList;
        QList<Hook*> m_signalHooks;
        QList<Hook*> m_slotHooks;
        QList<Hook*> m_propHooks;
        QMap<QString, Hook*> m_propHookMap;

        fComponent* m_fComp;
        Module* m_module;

        PropDialog* m_propDialog;
};
#endif
