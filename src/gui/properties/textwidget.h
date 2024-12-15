/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include "ui_textwidget.h"
#include "propwidget.h"

class PropDialog;
class Scripted;

class TextWidget : public PropWidget, private Ui::TextWidget
{
    Q_OBJECT
    
    public:
        TextWidget( PropDialog* parent, CompBase* comp, ComProperty* prop );
        ~TextWidget();
        
        virtual void setup( bool isComp ) override;
        virtual void updtValues() override;

    public slots:
        //void on_saveButton_clicked();
        void on_textBox_textChanged();

    private:
        void updatValue();

        //Scripted* m_scriptComp;
};

#endif
