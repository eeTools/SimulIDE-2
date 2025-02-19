/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "textwidget.h"
//#include "scripted.h"
#include "comproperty.h"
#include "propdialog.h"
//#include "mainwindow.h"

TextWidget::TextWidget( PropDialog* parent, ComProperty* prop )
          : PropWidget( parent, prop )
{
    setupUi(this);
}
TextWidget::~TextWidget() {}

void TextWidget::setup( bool )
{
    //float scale = MainWindow::self()->fontScale();
    //QFont font = textBox->font();
    //font.setPixelSize( 11.0*scale );
    //textBox->setFont( font );

    QString text = m_property->getValStr();
    text = text.replace("&#xa;", "\n").replace("&#x22;", "\"")
            .replace("&#x3D", "=").replace("&#x3C", "<").replace("&#x3E", ">");
    textBox->setText( text );
    updatValue();
    this->adjustSize();
}

/*void TextWidget::on_saveButton_clicked()
{
    QString script = textBox->toPlainText();
    QString excep  = m_scriptComp->evaluate( script );

    if( excep == "" )
    {
        excep = tr(" No Errors\n\n    Script Saved.");
        /// FIXME m_scriptComp->setProperty( m_propName.toUtf8(), script );
    }
    else excep += tr("\n\n    Script Not Saved.");

    evalText->setPlainText( excep );
}*/

void TextWidget::on_textBox_textChanged()
{
    m_property->setValStr( textBox->toPlainText() );
    m_propDialog->changed();
}

void TextWidget::updatValue()
{
    /// FIXME QString text = m_scriptComp->property( m_propName.toUtf8() ).toString();
    /// FIXME textBox->setPlainText( text );
}

void TextWidget::updtValues()
{

}
