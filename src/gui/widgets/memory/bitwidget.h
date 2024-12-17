/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BITWIDGET_H
#define BITWIDGET_H

#include <QWidget>

#include "ui_bitwidget.h"

class Watchable;

class BitWidget : public QWidget, private Ui::BitWidget
{
    Q_OBJECT

    public:
        BitWidget( QString name, QString type, Watchable* core, QWidget* parent=0 );

        void updateValue();

        void setValueInt( int val );
        void setBitNames( QStringList bits );

    private:
        void setupTable();

        QString m_name;
        QString m_type;

        QStringList m_bitNames;

        int m_bits;
        //int m_intVal;

        Watchable* m_core;
};

#endif
