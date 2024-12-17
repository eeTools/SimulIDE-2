/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef VALUEWIDGET_H
#define VALUEWIDGET_H

#include <QWidget>

#include "ui_valuewidget.h"

class Watchable;

class ValueWidget : public QWidget, private Ui::ValueWidget
{
    Q_OBJECT

    public:
        ValueWidget( QString name, QString type, Watchable* core, QWidget* parent=0 );

        void updateValue();

        void setBitNames( QString bits );

        void setValueInt( int val );
        void setValueStr( QString str );

    private:
        void setupTable();

        QString m_name;
        QString m_type;

        QStringList m_bitNames;

        QString m_strVal;

        int m_intVal;
        int m_bits;

        Watchable* m_core;
};

#endif
