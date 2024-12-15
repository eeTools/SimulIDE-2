/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef LABELWIDGET_H
#define LABELWIDGET_H

#include <QWidget>

#include "ui_labelwidget.h"

class LabelWidget : public QWidget, private Ui::LabelWidget
{
    Q_OBJECT
    
    public:
        LabelWidget( QWidget* parent );

        void setLabelVal( QString caption );
};

#endif
