/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "pathwidget.h"
#include "component.h"
#include "comproperty.h"
#include "propdialog.h"
#include "mainwindow.h"
#include "utils.h"

PathWidget::PathWidget( PropDialog* parent, ComProperty* prop )
          : PropWidget( parent, prop )
{
    setupUi(this);
}
PathWidget::~PathWidget() {}

void PathWidget::setup( bool )
{
    //float scale = MainWindow::self()->fontScale();
    //QFont font = valLabel->font();
    //font.setPixelSize( 11.0*scale );
    //valLabel->setFont( font );
    //value->setFont( font );
    //setPathButton->setFont( font );

    valLabel->setText( m_property->label() );
    updtValues();
    this->adjustSize();
}

void PathWidget::on_value_editingFinished()
{
    m_property->setValStr( value->text() );
    updtValues();
    m_propDialog->changed();
}

void PathWidget::on_setPathButton_clicked()
{
    QString path = getDirDialog( tr("Select directory"), value->text() );
    value->setText( path );
    on_value_editingFinished();
}

void PathWidget::updtValues()
{
    QString text = m_property->getValStr();
    value->setText( text );
}
