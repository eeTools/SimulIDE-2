/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QComboBox>

#include "propwidget.h"
#include "propdialog.h"
#include "comproperty.h"
#include "circuit.h"

PropWidget::PropWidget( PropDialog* parent, ComProperty* prop )
          : QWidget( parent )
{
    m_propDialog = parent;
    m_component = prop->compBase();
    m_property  = prop;
    m_propId  = prop->idStr();
    m_blocked = false;
}
PropWidget::~PropWidget() {}

void PropWidget::prepareChange()
{
    m_undo = m_property->flags() & P_NoCopy;
    if( !m_undo ) return;
    m_oldValue = m_property->getValStr();
    Circuit::self()->beginUndoStep();
}

void PropWidget::saveChanges()
{
    if( m_undo ){ // Don'use endUndoStep() because We need addItemChange() before endChangeBatch()
        Circuit::self()->calculateChanges();
        Circuit::self()->addItemChange( m_component->getUid(), m_propId, m_oldValue );
        Circuit::self()->endChangeBatch();
    }
    else if( m_propDialog ) m_propDialog->changed();

    if( m_propDialog ) m_propDialog->updtValues();
    else               updtValues();
}

#include "moc_propwidget.cpp"
