/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef STRWIDGET_H
#define STRWIDGET_H

#include "ui_strwidget.h"
#include "propwidget.h"

class Component;
class PropDialog;

class StrWidget : public PropWidget, private Ui::StrWidget
{
    Q_OBJECT
    
    public:
        StrWidget( PropDialog* parent, ComProperty* prop );
        ~StrWidget();

        virtual void setup( bool ) override;
        virtual void updtValues() override;
        virtual void updateName() override;

    public slots:
        void on_value_editingFinished();
};

#endif
