/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "enumwidget.h"
#include "component.h"
#include "propdialog.h"
#include "comproperty.h"

EnumWidget::EnumWidget( PropDialog* parent, CompBase* comp, ComProperty* prop )
          : PropWidget( parent, comp, prop )
{
    setupUi(this);
}
EnumWidget::~EnumWidget() {}

void EnumWidget::setup( bool isComp )
{
    valLabel->setText( m_property->name() );
    m_blocked = true;

    QStringList enumNames = m_component->getEnumNames( m_propId );/// enums.sort();
    for( QString val : enumNames ) valueBox->addItem( val );

    m_enums = m_component->getEnumUids( m_propId );

    QString valStr = m_property->getValStr();
    valueBox->setCurrentIndex( m_enums.indexOf( valStr) );

    if( !isComp ) showVal->setVisible( false );

    m_blocked = false;
    updtValues();
    this->adjustSize();
}

void EnumWidget::on_showVal_toggled( bool checked )
{
    if( m_blocked ) return;
    m_blocked = true;

    if( checked ) m_component->setPropStr("ShowProp", m_propId );
    else          m_component->setPropStr("ShowProp", "" );
    if( m_propDialog ) {
        m_propDialog->updtValues();
        m_propDialog->changed();
    }
    else updtValues();
    m_blocked = false;
}

void EnumWidget::on_valueBox_currentIndexChanged( QString val )
{
    if( m_blocked ) return;

    prepareChange();
    int index = valueBox->currentIndex();
    m_property->setValStr( m_enums.at( index ) );
    saveChanges();
}

void EnumWidget::updtValues()
{
    if( m_blocked ) return;
    m_blocked = true;
    showVal->setChecked( m_component->getPropStr("ShowProp") == m_propId );
    m_blocked = false;
}

