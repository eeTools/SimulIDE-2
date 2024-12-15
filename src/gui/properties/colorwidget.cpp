/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QColorDialog>

#include "colorwidget.h"
#include "component.h"
#include "comproperty.h"
#include "propdialog.h"

ColorWidget::ColorWidget( PropDialog* parent, CompBase* comp, ComProperty* prop )
           : PropWidget( parent, comp, prop )
{
    setupUi(this);
}
ColorWidget::~ColorWidget(){}

void ColorWidget::setup( bool )
{
    valLabel->setText( m_property->name() );

    m_color = m_property->getValStr(); /// FIXME // comp->property( m_propId.toUtf8() ).value<QColor>();
    QString cname = m_color.name();
    int index = 0;

    valueBox->addItem( "Custom" );

    QStringList colorNames = QColor::colorNames();
    for( int i=0; i<colorNames.size(); ++i )
    {
        QString colorName = colorNames[i];
        QColor color( colorName );
        if( color.name() == cname ) index = i+1;
        valueBox->addItem( colorName );
        valueBox->setItemData( i+1, color, Qt::DecorationRole );
    }
    valueBox->setEditable( true );
    valueBox->setCurrentIndex( index );
    QPalette pal = colorW->palette();
    pal.setColor( QPalette::Base, m_color );
    colorW->setPalette( pal );

    this->adjustSize();
}

void ColorWidget::on_valueBox_currentIndexChanged( int index )
{
    if( !m_component ) return;

    QString value = valueBox->itemText( index );
    if( value == "Custom" )
    {
        QColor color = QColorDialog::getColor( m_color, this );
        if( color.isValid() ){
            m_color = color;
            valueBox->setItemData( 0, color, Qt::DecorationRole );
        }
    }
    else m_color = QColor( value );

    m_component->setPropStr( m_propId, m_color.name() ); /// FIXME

    QPalette pal = colorW->palette();
    pal.setColor( QPalette::Base, m_color );
    colorW->setPalette( pal );

    if( m_propDialog ) m_propDialog->changed();
}

void ColorWidget::updtValues()
{

}

