/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PROPWIDGET_H
#define PROPWIDGET_H

#include <QWidget>

class CompBase;
class PropDialog;
class QComboBox;
class ComProperty;

class PropWidget : public QWidget
{
    public:
        PropWidget( PropDialog* parent, CompBase* comp, ComProperty* prop );
        ~PropWidget();

        virtual void setup( bool isComp )=0;
        virtual void updtValues() {;}
        virtual void updateName() {;}

        QString propId() { return m_propId; }

    protected:
        void prepareChange();
        void saveChanges();

        bool m_blocked;
        bool m_undo;

        QString m_oldValue;
        QString m_propId;

        CompBase*    m_component;
        ComProperty* m_property;
        PropDialog*  m_propDialog;
};
#endif
