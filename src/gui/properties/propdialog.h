/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PROPDIALOG_H
#define PROPDIALOG_H

#include <QDialog>

#include "ui_propdialog.h"

class CompBase;
class PropWidget;

class PropDialog : public QDialog, private Ui::PropDialog
{
    Q_OBJECT
    
    public:
        PropDialog( QWidget* parent=0, QString help="" );

        void setComponent( CompBase* comp, bool isComp=true, bool showHelp=true );
        void showProp( QString name, bool show );
        void enableProp( QString name, bool en );
        void updtValues();

        void changed();

        PropWidget* getPropWidget( QString propName );
        //QList<Propwidget*> propWidgets() { return m_propList; }

    public slots:
        void on_labelBox_editingFinished();
        void on_showButton_toggled( bool checked );
        void on_tabList_currentChanged( int );
        void on_helpButton_clicked();
        void adjustWidgets();

    private:
        CompBase* m_component;

        bool m_helpExpanded;

        float m_scale;
        int m_minW;
        int m_minH;

        QList<PropWidget*> m_propList;
};

#endif
