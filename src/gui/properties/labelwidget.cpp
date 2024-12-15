/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "labelwidget.h"

LabelWidget::LabelWidget( QWidget* parent )
           : QWidget( parent )
{
    setupUi( this );
}

void LabelWidget::setLabelVal( QString caption )
{
    label->setText( caption );
}

