/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PATHWIDGET_H
#define PATHWIDGET_H

#include "ui_pathwidget.h"
#include "propwidget.h"

class Component;
class PropDialog;

class PathWidget : public PropWidget, private Ui::PathWidget
{
    Q_OBJECT
    
    public:
        PathWidget( PropDialog* parent, CompBase* comp, ComProperty* prop );
        ~PathWidget();

        virtual void setup( bool ) override;
        virtual void updtValues() override;

    public slots:
        void on_value_editingFinished();
        void on_setPathButton_clicked();
};

#endif
