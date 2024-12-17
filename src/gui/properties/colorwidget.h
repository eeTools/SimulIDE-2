/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include "ui_colorwidget.h"
#include "propwidget.h"

class CompBase;
class PropDialog;

class ColorWidget : public PropWidget, private Ui::ColorWidget
{
    Q_OBJECT
    
    public:
        ColorWidget( PropDialog* parent, CompBase* comp, ComProperty* prop );
        ~ColorWidget();

        void setup( bool ) override;
        void updtValues() override;

    public slots:
        void on_valueBox_currentIndexChanged( int index );

    private:
        QColor m_color;
};

#endif
