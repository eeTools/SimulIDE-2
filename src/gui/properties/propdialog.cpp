/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "propdialog.h"
#include "component.h"
#include "circuit.h"

#include "labelwidget.h"
#include "numwidget.h"
#include "pathwidget.h"
#include "strwidget.h"
#include "textwidget.h"
#include "enumwidget.h"
#include "boolwidget.h"
#include "colorwidget.h"
#include "iconwidget.h"
#include "mainwindow.h"

#include "comproperty.h"

PropDialog::PropDialog( QWidget* parent, QString help )
          : QDialog( parent )
{
    setupUi( this );
    //this->setWindowFlags( Qt::Dialog | Qt::WindowTitleHint );

    m_component = nullptr;

    m_helpExpanded = false;
    mainLayout->removeWidget( helpText );
    helpText->setText( help );
    helpText->adjustSize();
    helpText->setVisible( false );
}

void PropDialog::setComponent( CompBase* comp, bool isComp, bool showHelp )
{
    QFontMetrics fm( labelLabel->font() );
    m_scale = fm.width(" ")/2.0;
    m_minW  = 265*m_scale;
    m_minH  = 100*m_scale;

    QString title = isComp ? "Uid: " : "";
    this->setWindowTitle( title+QString::number( comp->getUid() ) );
    type->setText( "Type: "+comp->itemType() );
    if( isComp ) labelBox->setText( comp->getPropStr("label") );
    else{
        labelLabel->setVisible( false );
        labelBox->setVisible( false );
        showButton->setVisible( false );
        labelShow->setVisible( false );
    }
    if( !showHelp )
    {
        helpButton->setVisible( false );
        type->setVisible( false );
    }
    tabList->clear();
    m_component = comp;
    showButton->setChecked( comp->getPropStr("Show_id") == "true" );

    int index=0;
    QList<propGroup> groups = comp->m_propGroups;

    /*if( isComp ){
        std::vector<property_t> props = comp->getProperties();
        *props.at(0).value = 100;
    }*/

    for( propGroup group : groups )
    {
        if( group.flags & groupHidden ) continue;
        if( group.flags & groupPkg && isComp ) continue;

        bool groupEnabled = true;
        bool isMaincomp = comp->isHidden() && (comp->getPropStr("mainComp") == "true"); // main Compoenent & is in subcircuit
        if( isMaincomp && (group.flags & groupNoCopy) ) groupEnabled = false;

        QList<ComProperty*> propList = group.propList;
        if( !propList.isEmpty() )
        {
            index++;
            QWidget* groupWidget = new QWidget( tabList );
            groupWidget->setLayout( new QVBoxLayout( groupWidget ));
            groupWidget->layout()->setSpacing( 9 );
            groupWidget->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
            groupWidget->setObjectName( group.name );

            for( ComProperty* prop : propList )
            {
                if( prop->flags() & P_Hidden ) continue; // Property hidden
                 //|| prop->flags() & propSignal
                 //|| prop->flags() & propSlot ) continue; // Property hidden

                if( prop->idStr() == "" ) // Just a label
                {
                    QString text = prop->label();
                    if( text == "separator")
                    {
                        QFrame* line = new QFrame;
                        line->setFrameShape( QFrame::HLine );
                        line->setFrameShadow( QFrame::Sunken );
                        groupWidget->layout()->addWidget( line );
                    }else{
                        LabelWidget* mp = new LabelWidget( this );
                        mp->setLabelVal( text );
                        groupWidget->layout()->addWidget( mp );
                    }
                    continue;
                }

                PropWidget* mp = prop->getWidget();
                if( !mp ) continue;

                mp->setup( isComp );
                m_propList.append( mp );
                groupWidget->layout()->addWidget( mp );

                bool propEnabled = true;
                if( isMaincomp && (prop->flags() & P_NoCopy) ) propEnabled = false;

                mp->setEnabled( groupEnabled && propEnabled );
            }
            tabList->addTab( groupWidget, group.name );
    }   }
    if( tabList->count() == 0 ) tabList->setVisible( false ); // Hide tab widget if empty
}

PropWidget* PropDialog::getPropWidget( QString propName )
{
    for( PropWidget* widget : m_propList )
        if( widget->propId() == propName ) return widget;

    return nullptr;
}

void PropDialog::showProp( QString name, bool show )
{
    for( PropWidget* prop : m_propList )
    {
        if( prop->propId() != name ) continue;
        prop->setHidden( !show );
        if( show ) this->adjustSize();
        break;
    }
}

void PropDialog::enableProp( QString name, bool en )
{
    for( PropWidget* prop : m_propList )
    {
        if( prop->propId() != name ) continue;
        prop->setEnabled( en );
        break;
    }
}

void PropDialog::on_labelBox_editingFinished()
{
    m_component->setPropStr("label", labelBox->text() );
}

void PropDialog::on_showButton_toggled( bool checked )
{
    QString show = checked ? "true" : "false";
    m_component->setPropStr("Show_id", show );
}

void PropDialog::on_tabList_currentChanged( int )
{
    if( !m_component ) return;
    updtValues();
    adjustWidgets();
}

void PropDialog::on_helpButton_clicked()
{
    m_helpExpanded = !m_helpExpanded;
    if( m_helpExpanded ) mainLayout->addWidget( helpText );
    else                 mainLayout->removeWidget( helpText );
    helpText->setVisible( m_helpExpanded );

    adjustWidgets();
}

void PropDialog::adjustWidgets()
{
    int h = 0;
    int w = m_minW;
    if( helpText->isVisible() ) w += helpText->width()+6;

    QWidget* widget = tabList->currentWidget();
    if( widget )
    {
        int count = 0;
        for( int i=0; i<widget->layout()->count(); ++i )
            if( !widget->layout()->itemAt(i)->widget()->isHidden() )
                count++;

        h = count*24*m_scale+90*m_scale;
    }
    if( h < m_minH ) h = m_minH;

    this->setFixedHeight( h );
    this->setMaximumWidth( w+100*m_scale );
    this->adjustSize();
    this->setMaximumHeight( h+70*m_scale );
}

void PropDialog::updtValues()
{
    for( PropWidget* prop : m_propList ) prop->updtValues();
}

void PropDialog::changed()
{
    Circuit::self()->setChanged();
}
