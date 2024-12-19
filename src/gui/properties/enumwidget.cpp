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
    valLabel->setText( m_property->label() );
    m_blocked = true;

    setEnums( m_property->unit() );

    QString valStr = m_property->getValStr();
    valueBox->setCurrentIndex( m_enums.indexOf( valStr) );

    QFontMetrics fm( valueBox->font() );
    float scale = fm.width(" ")/2;
    valueBox->setFixedWidth( 100.0*scale );

    if( !isComp ) showVal->setVisible( false );

    m_blocked = false;
    updtValues();
    this->adjustSize();
}

void EnumWidget::setEnums( QString e )
{
    QStringList list = e.split(";");
    m_enums = list.takeFirst().split(",");
    m_enums.removeAll("");

    QStringList enumNames;
    if( list.size() ) enumNames = list.first().split(",");
    else              enumNames = m_enums;
    enumNames.removeAll("");

    bool isBlocked = m_blocked;
    m_blocked = true;
    valueBox->clear();
    for( QString val : enumNames ) valueBox->addItem( val );
    m_blocked = isBlocked;
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
    QString valStr = m_property->getValStr();
    valueBox->setCurrentIndex( m_enums.indexOf( valStr) );
    showVal->setChecked( m_component->getPropStr("ShowProp") == m_propId );
    m_blocked = false;
}

