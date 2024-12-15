/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef COMPOSERWIDGET_H
#define COMPOSERWIDGET_H

#include <QMenu>
#include <QToolBar>
#include <QSplitter>
#include <QVBoxLayout>

#include "composerview.h"
#include "canvasview.h"

class PropDialog;
class fComponent;

class ListItem : public CompBase
{
    public:
        ListItem() : CompBase("item") {;}

        QString label() { return m_label; }
        void setLabel( QString l ) { m_label = l; }

        QString category() { return m_category;}
        void setCategory( QString c ) { m_category = c; }

        QString icon() { return m_icon;}
        void setIcon( QString i ) { m_icon = i; }

    private:
        QString m_label;
        QString m_category;
        QString m_icon;
};

class ComposerWidget : public QWidget
{
    public:
        ComposerWidget( QWidget* parent );
        ~ComposerWidget();

        fComponent* package() { return m_fComp; }

 static ComposerWidget* self() { return m_pSelf; }

    public slots:
        bool newCanvas();
        void openRecentFile();
        void open();
        void load( QString path );
        void save( QString file );
        void save();
        void saveAs();
        void settApp();

    private:
        void createActions();
        void createToolBars();
        void updtRecentFiles();
        void updtRecentFileActions();
        void createPackage();
        bool saveString( QString fileName, QString doc );

 static ComposerWidget* m_pSelf;

        fComponent* m_fComp;
        ListItem m_listItem;

        QString m_currentFile;
        QString m_compFolder;

        QVBoxLayout  m_vLayout;
        CanvasView   m_packageView;
        ComposerView m_composerView;
        QToolBar     m_toolBar;
        QMenu        m_fileMenu;

        PropDialog* m_pkgProps;


        QSplitter* m_mainSplitter;
        QSplitter* m_panelSplitter;

        enum { MaxRecentFiles = 10 };

        QAction* recentFileActs[MaxRecentFiles];
        QAction* newAct;
        QAction* openAct;
        QAction* saveAct;
        QAction* saveAsAct;
        QAction* zoomFitAct;
        QAction* zoomSelAct;
        QAction* zoomOneAct;
        QAction* settAppAct;
};
#endif
