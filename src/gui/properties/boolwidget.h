/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BOOLWIDGET_H
#define BOOLWIDGET_H

#include "ui_boolwidget.h"
#include "propwidget.h"

class Component;
class PropDialog;

class BoolWidget : public PropWidget, private Ui::BoolWidget
{
    Q_OBJECT
    
    public:
        BoolWidget( PropDialog* parent, CompBase* comp, ComProperty* prop );
        ~BoolWidget();
        
        virtual void setup( bool ) override;
        virtual void updtValues() override;

    public slots:
        void on_trueVal_toggled( bool checked );
};

#endif
