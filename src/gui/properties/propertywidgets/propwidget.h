/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QWidget>

class CompBase;
class PropDialog;
class QComboBox;
class ComProperty;

class PropWidget : public QWidget
{
    Q_OBJECT

    public:
        PropWidget( PropDialog* parent, ComProperty* prop );
        ~PropWidget();

        virtual void setup( bool isComp )=0;
        virtual void updtValues() {;}
        virtual void updateName() {;}

        QString propId() { return m_propId; }

    signals:
        void showToggled( QString propId, bool checked );

    protected:
        void prepareChange();
        void saveChanges();

        bool m_blocked;
        bool m_undo;

        QString m_oldValue;
        QString m_propId;

        CompBase*    m_compBase;
        ComProperty* m_property;
        PropDialog*  m_propDialog;
};
