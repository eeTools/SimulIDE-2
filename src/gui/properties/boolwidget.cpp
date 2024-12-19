/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "boolwidget.h"
#include "component.h"
#include "propdialog.h"
#include "comproperty.h"

BoolWidget::BoolWidget( PropDialog* parent, CompBase* comp, ComProperty* prop )
          : PropWidget( parent, comp, prop )
{
    setupUi(this);
}
BoolWidget::~BoolWidget() {;}

void BoolWidget::setup( bool )
{
    trueVal->setText( m_property->label() );
    m_blocked = true;

    bool checked = (m_property->getValStr() == "1");
    trueVal->setChecked( checked );
    m_blocked = false;

    this->adjustSize();
}

void BoolWidget::on_trueVal_toggled( bool checked )
{
    if( m_blocked ) return;

    prepareChange();
    m_property->setValStr( checked ? "1" : "0" );
    saveChanges();
}

void BoolWidget::updtValues()
{
    m_blocked = true;
    bool checked = (m_property->getValStr() == "1");
    trueVal->setChecked( checked );
    m_blocked = false;
}
