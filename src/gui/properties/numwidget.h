/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef NUMWIDGET_H
#define NUMWIDGET_H

#include "ui_numwidget.h"
#include "propwidget.h"

class Component;
class PropDialog;

class NumWidget : public PropWidget, private Ui::NumWidget
{
    Q_OBJECT
    
    public:
        NumWidget( PropDialog* parent, CompBase* comp, ComProperty* prop );
        ~NumWidget();

        virtual void setup( bool isComp ) override;
        virtual void updtValues() override;
        virtual void updateName() override;

    public slots:
        void on_showVal_toggled( bool checked );
        void on_valueBox_valueChanged( double val );

    private:
        QString m_type;

        bool m_useMult;
};

#endif
