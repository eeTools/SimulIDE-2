/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef ENUMWIDGET_H
#define ENUMWIDGET_H

#include "ui_enumwidget.h"
#include "propwidget.h"

class Component;
class PropDialog;

class EnumWidget : public PropWidget, private Ui::EnumWidget
{
    Q_OBJECT
    
    public:
        EnumWidget( PropDialog* parent, ComProperty* prop );
        ~EnumWidget();

        virtual void setup( bool isComp ) override;
        virtual void updtValues() override;

        void setEnums( QString e );

    public slots:
        void on_showVal_toggled( bool checked );
        void on_valueBox_currentIndexChanged( QString val );

    protected:
        QStringList m_enums;
};

#endif
