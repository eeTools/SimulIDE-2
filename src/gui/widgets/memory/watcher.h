/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef WATCHER_H
#define WATCHER_H

#include <QWidget>

#include "ui_watcher.h"

class ValueWidget;
class QStandardItemModel;
class ScriptCpu;
class Console;
class Watchable;

class Watcher : public QWidget, private Ui::Watcher
{
    Q_OBJECT

    public:
        Watcher( QWidget* parent=nullptr, Watchable* cpu=nullptr );

        void updateValues();

        void addRegisters( QStringList regs );
        void addRegister( QString name, QString type, QString bits="" );

        void setVariables( QStringList vars );
        void addVariable( QString name, QString type );
        //void addbitField( QString name, QString type, QStringList bits );

        void loadVarSet( QStringList varSet );
        QStringList getVarSet();

        void addConsole();
        Console* console() { return m_console; }

        void addWidget( QWidget* widget );

    public slots:
        void RegDoubleClick( const QModelIndex& index );
        void VarDoubleClick( const QModelIndex& index );

    private:
        void addHeader();
        void insertValue( QString name );

        bool m_header;

        Watchable* m_core;
        Console* m_console;

        QStandardItemModel* m_registerModel;
        QStandardItemModel* m_variableModel;

        QMap<QString, QString> m_typeTable;
        QMap<QString, QString> m_bitTable;
        QMap<QString, ValueWidget*> m_values;

        QBoxLayout* m_valuesLayout;
};

#endif
