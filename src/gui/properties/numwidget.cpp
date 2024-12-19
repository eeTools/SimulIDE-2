/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "numwidget.h"
#include "component.h"
#include "propdialog.h"
#include "comproperty.h"
#include "utils.h"

NumWidget::NumWidget( PropDialog* parent, CompBase* comp, ComProperty* prop )
         : PropWidget( parent, comp, prop )
{
    setupUi(this);

    QString unit = prop->unit();

    if( unit.isEmpty() || unit.startsWith("_") )
    {
        m_useMult = false;
        if( unit.startsWith("_") ) valueBox->setSuffix( unit.replace("_", " ") );
    }else{
        m_useMult = true;
        /// valueBox->setUnitStr( unit );
        valueBox->setSuffix( " "+unit );
    }

    m_type = prop->type();
    if( m_type == "double" ) return;

    if( m_type == "uint" ) valueBox->setMinimum( 0 );
    valueBox->setDecimals( 0 );

}
NumWidget::~NumWidget() {}

void NumWidget::setup( bool isComp )
{
    valLabel->setText( m_property->label() );
    m_blocked = true;

    if( !isComp )
    {
        showVal->setVisible( false );
        if( (m_property->flags() & propHidden) )
            valueBox->setVisible( false );
    }

    m_blocked = false;
    updtValues();
}

void NumWidget::updateName()
{
    valLabel->setText( m_property->label() );
}

void NumWidget::on_showVal_toggled( bool checked )
{
    if( m_blocked ) return;
    m_blocked = true;

    if( checked ) m_component->setPropStr("ShowProp", m_propId );
    else          m_component->setPropStr("ShowProp", "" );

    /// m_propDialog->updtValues();
    if( m_propDialog ) m_propDialog->changed();
    m_blocked = false;
}

void NumWidget::on_valueBox_valueChanged( double val )
{
    if( m_blocked ) return;
    m_blocked = true;

    prepareChange();

    if( m_useMult ) m_property->setValStr( valueBox->text() );
    else            m_property->setValStr( QString::number( val ) );

    m_blocked = false;
    if( m_propDialog ) m_propDialog->updtValues();
    else               updtValues();
    saveChanges();
}

void NumWidget::updtValues()
{
    if( m_blocked ) return;
    m_blocked = true;

    /// showVal->setChecked( m_component->getPropStr("ShowProp") == m_propId );

    double multiplier = 1;
    if( m_useMult ) multiplier = getMultiplier( valueBox->suffix() );

    double val = m_property->getValue()/multiplier;
    if( valueBox->value() != val ) valueBox->setValue( val );

    m_blocked = false;
}

