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
        FuncBlock( fComponent* fComp, QString type, int id );
        ~FuncBlock();

        enum { Type = UserType + 10 };
        int type() const override { return Type; }

        QRectF boundingRect() const override { return QRectF( m_area.x()-2, m_area.y()-2, m_area.width()+4, m_area.height()+4 ); }

        void remove();

        void move( QPointF delta );

        void updateWidget();

        QString toString();

        int uid() { return m_id; }

        Module* module() { return m_module; }

        void renamePropHooks( QString oldName, QString newName );

        void setPropStr( QString name, QString value );

    protected slots:
        virtual void slotProperties();
        void showToggled( QString propId, bool checked );

    protected:
        void paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* ) override;
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
        int m_id;

        QRectF m_area;
        QRectF m_bodyArea;
        QRectF m_headArea;
        QRectF m_PropArea;

        QColor m_color;
        QColor m_hColor;
        QColor m_pColor;

        QFont m_font;

        QList<FuncBlock*> m_blockMoveList;
        QList<Hook*> m_signalHooks;
        QList<Hook*> m_slotHooks;

        QList<Hook*> m_propSlots;
        QMap<QString, Hook*> m_propSlotMap;

        QList<Hook*> m_propSignals;
        QMap<QString, Hook*> m_propSignalMap;

        Module* m_module;  /// TODO: split Module into GUI and functional parts

        PropDialog* m_propDialog;
};
#endif
