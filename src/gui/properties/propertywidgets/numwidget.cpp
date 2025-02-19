/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

//#include <QDebug>

#include "numwidget.h"
#include "component.h"
#include "propdialog.h"
#include "doubprop.h"
#include "intprop.h"
#include "utils.h"

NumWidget::NumWidget( PropDialog* parent, ComProperty* prop )
         : PropWidget( parent, prop )
{
    setupUi(this);

    QString unit = prop->unit();

    m_useMult = !unit.isEmpty();

    if( m_useMult )
    {
        //if( unit.startsWith("_") ) unit.replace("_", "");//valueBox->setSuffix( unit.replace("_", " ") );

        valueBox->setUnitStr( unit );
        valueBox->setSuffix( " "+unit );
    }

    m_type = prop->type();
    if( m_type == P_Double ) return;

    valueBox->setDecimals( 0 );
    if( m_type == P_Uint ) valueBox->setMinimum( 0 );
}
NumWidget::~NumWidget() {}

void NumWidget::setup( bool isComp )
{
    valLabel->setText( m_property->label() );
    m_blocked = true;

    if( !isComp )
    {
        showVal->setVisible( false );
        if( (m_property->flags() & P_Hidden) )
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

    emit showToggled( m_propId, checked );

    m_blocked = false;
}

void NumWidget::on_valueBox_textChanged( const QString &text )
{
    if( m_blocked ) return;
    m_blocked = true;

    prepareChange();

    m_property->setValStr( text );

    m_blocked = false;
    saveChanges();
}

void NumWidget::updtValues()
{
    if( m_blocked ) return;
    m_blocked = true;

    double value = 0;
    if( m_property->type() == P_Double )
    {
        double multiplier = 1;
        if( m_useMult ) multiplier = getMultiplier( static_cast<DoubProp*>(m_property)->m_multStr );

        value = *m_property->value().dblVal/multiplier;
    }
    else value = *m_property->value().intVal;

    if( valueBox->value() != value ) valueBox->setValue( value );

    m_blocked = false;
}

