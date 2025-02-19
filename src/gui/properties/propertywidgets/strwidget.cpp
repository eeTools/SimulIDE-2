/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "strwidget.h"
#include "component.h"
#include "comproperty.h"
#include "propdialog.h"

StrWidget::StrWidget( PropDialog* parent, ComProperty* prop )
         : PropWidget( parent, prop )
{
    setupUi(this);
}
StrWidget::~StrWidget() {}

void StrWidget::setup( bool )
{
    valLabel->setText( m_property->label() );
    updtValues();
    this->adjustSize();
}

void StrWidget::updateName()
{
    valLabel->setText( m_property->label() );
}

void StrWidget::on_value_editingFinished()
{
    prepareChange();
    m_property->setValStr( value->text() );
    saveChanges();
}

void StrWidget::updtValues()
{
    QString text = m_property->getValStr();
    value->setText( text );
}
