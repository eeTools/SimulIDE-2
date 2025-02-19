/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include "ui_iconwidget.h"
#include "propwidget.h"

class CompBase;
class PropDialog;

class IconWidget : public PropWidget, private Ui::IconWidget
{
    Q_OBJECT

    struct iconItem_t{
        QString iconFile;
        QString iconData;
    };
    
    public:
        IconWidget( PropDialog* parent, ComProperty* prop );
        ~IconWidget();

        void setup( bool ) override;
        void updtValues() override;

    public slots:
        void on_iconChoose_clicked();
        void on_iconBox_currentIndexChanged( QString val );

    private:
        void addIcon( iconItem_t iconItem );
        void addIconFile( QString iconFile );

        QString m_circuitPath;
        QString m_iconFile;

        QList<iconItem_t> m_itemList;
};

#endif
