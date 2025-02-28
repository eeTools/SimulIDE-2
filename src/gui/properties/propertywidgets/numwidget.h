/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "ui_numwidget.h"
#include "propwidget.h"

class PropDialog;
class Label;

class NumWidget : public PropWidget, private Ui::NumWidget
{
    Q_OBJECT
    
    public:
        NumWidget( PropDialog* parent, ComProperty* prop );
        ~NumWidget();

        virtual void setup( bool isComp ) override;
        virtual void updtValues() override;
        virtual void updateName() override;

    public slots:
        void on_showVal_toggled( bool checked );
        //void on_valueBox_valueChanged( double val );
        void on_valueBox_textChanged( const QString &text );

    private:
        QString m_type;

        bool m_useMult;

        Label* m_compLabel;
};
