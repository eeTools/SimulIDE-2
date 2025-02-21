/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>
#include <QSettings>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>

#include "composerwidget.h"
#include "mainwindow.h"
#include "propdialog.h"
#include "fcomponent.h"
#include "canvas.h"
#include "utils.h"

ComposerWidget* ComposerWidget::m_pSelf = nullptr;

ComposerWidget::ComposerWidget( QWidget* parent  )
              : QWidget( parent )
              , m_vLayout( this )
              , m_packageView( this )  // Create before composer
              , m_composerView( this )
              , m_toolBar( this )
              , m_fileMenu( this )
{
    m_pSelf = this;

    m_pkgProps = nullptr;
    m_currentFile = "";

    m_vLayout.setContentsMargins(0, 0, 0, 0);
    m_vLayout.setSpacing(0);
    m_vLayout.addWidget( &m_toolBar );

    m_mainSplitter = new QSplitter( this );
    m_mainSplitter->setOrientation( Qt::Horizontal );
    m_vLayout.addWidget( m_mainSplitter );

    m_panelSplitter = new QSplitter( this );
    m_panelSplitter->setOrientation( Qt::Vertical );
    m_panelSplitter->addWidget( &m_packageView );
    //m_panelSplitter->addWidget( &m_configTabs );

    m_mainSplitter->addWidget( &m_composerView );
    m_mainSplitter->addWidget( m_panelSplitter );

    m_mainSplitter->setSizes( {500, 200} );
    m_panelSplitter->setSizes( {500, 500} );

    m_compFolder = MainWindow::self()->userPath()+"Components"; //settings()->value("Composer/lastFile").toByteArray();

    createActions();
    createToolBars();
    updtRecentFileActions();
    newCanvas();
}
ComposerWidget::~ComposerWidget(){}

void ComposerWidget::createPackage()
{
    m_packageView.clear();

    m_fComp = new fComponent("",0, Canvas::self() );
    m_fComp->setPos(-4,-4 );

    Canvas::self()->addItem( m_fComp );

    /*m_fComp->addPropGroup( { tr("List Item"),
    {
        new StrProp("label"   , tr("Label"), "", &m_listItem, 0 ),
        new StrProp("category", tr("Category"), "", &m_listItem, 0 ),
        new StrProp("type"    , tr("Type"), "", &m_listItem, 0 ),
        new StrProp("icondata", tr("Icon"), "", &m_listItem, 0, "icon" ),
    },0} );*/

    if( m_pkgProps ) m_pkgProps->close();
    m_pkgProps = new PropDialog( this, "" );
    m_pkgProps->setComponent( m_fComp, false, false );
    m_pkgProps->updtValues();

    m_panelSplitter->addWidget( m_pkgProps );
}

bool ComposerWidget::newCanvas()
{
    if( MainWindow::self()->windowTitle().endsWith('*') )
    {
        const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, "ComposerWidget::newCanvas"
                               , tr("\nComponent has been modified")+".\n"
                               + tr("Do you want to save your changes?")+".\n",
          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if     ( ret == QMessageBox::Save )   save();
        else if( ret == QMessageBox::Cancel ) return false;
    }
    m_composerView.clear();

    createPackage();
    m_currentFile = "";

    MainWindow::self()->setFile( tr("New Component") );
    MainWindow::self()->settings()->setValue("Composer/lastFile", m_compFolder );

    return true;
}

void ComposerWidget::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if( !action ) return;

    QString file = action->data().toString();
    QFile pfile( file );
    if( pfile.exists() ) load( file );
    else{
        const QMessageBox::StandardButton ret
        = QMessageBox::warning( this, "ComposerWidget::openRecentFile",
                               tr("\nCan't find file:\n")+
                               file+"\n\n"+
                               tr("Do you want to remove it from Recent files?\n"),
          QMessageBox::Yes | QMessageBox::No );

        if( ret == QMessageBox::Yes )
        {
            QSettings* settings = MainWindow::self()->settings();
            QStringList files = settings->value("Composer/recentFiles").toStringList();
            files.removeAll( file );
            settings->setValue("Composer/recentFiles", files );
            updtRecentFileActions();
        }
    }
}

void ComposerWidget::open()
{
    const QString dir = m_compFolder;
    QString fileName = QFileDialog::getOpenFileName( 0l, tr("Load Component"), dir
                        , tr("Components")+" (*.comp*);;"+tr("All files")+" (*.*)");
    load( fileName );
}

void ComposerWidget::load( QString path )
{
    if( !path.isEmpty() && path.endsWith(".comp") )
    {
        createPackage();

        Composer::self()->loadComponent( path );
        m_pkgProps->updtValues();

        CanvasView::self()->zoomToFit();
        ComposerView::self()->zoomToFit();

        m_currentFile = path;
        m_compFolder = path;
        MainWindow::self()->setFile(path.split("/").last());

        QSettings* settings = MainWindow::self()->settings();
        settings->setValue( "Composer/lastFile", m_compFolder );

        updtRecentFiles();
    }
}

void ComposerWidget::save()
{
    if( m_currentFile.isEmpty() ) saveAs();
    else save( m_currentFile );
}

void ComposerWidget::save( QString file )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    if( !file.endsWith(".comp") ) file.append(".comp");

    Composer::self()->deleteNewWire();

    QString compStr= "Component";
    //component += "; uid=0";
    compStr += "; uid=0";
    compStr += "; starthalf=" + QString::number( m_fComp->startHalf() );
    compStr += "; width="     + QString::number( m_fComp->width() );
    compStr += "; height="    + QString::number( m_fComp->height() );
    compStr += "; shape="     + m_fComp->shapeStr();
    compStr += "; color="     + m_fComp->colorStr();
    compStr += "; background="+ m_fComp->background();
    compStr += "; type="      + m_listItem.itemType();
    compStr += "; label="     + m_listItem.label();
    compStr += "; category="  + m_listItem.category();
    compStr += "; icondata="  + m_listItem.icon();
    compStr += "; version="   + QString( APP_VERSION );
    compStr += "; rev="       + QString( REVNO );
    compStr += "\n";

    compStr += Composer::self()->toString();

    if( saveString( file, compStr ) )
    {
        m_currentFile = file;
        //QFile file( m_backupPath );
        //if( file.exists() ) QFile::remove( m_backupPath ); // Remove backup file

        m_compFolder = file;
        MainWindow::self()->setFile( file.split("/").last() );
        MainWindow::self()->settings()->setValue("Composer/lastFile", m_compFolder );
        updtRecentFiles();

        qDebug() << Qt::endl << "Componerentt Saved:"<< Qt::endl << file;
    }
    else qDebug() << "\nError Saving Component:\n" << file;

    QApplication::restoreOverrideCursor();
}

bool ComposerWidget::saveString( QString fileName, QString doc )
{
    QFile file( fileName );
    if( !file.open( QFile::WriteOnly | QFile::Text ))
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning( nullptr, "Circuit::saveString",
        tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << doc;
    file.close();

    return true;
}

void ComposerWidget::saveAs()
{
    const QString dir = m_compFolder;
    QString fileName =
        QFileDialog::getSaveFileName( this, tr("Save Component"), dir,
                                  tr("Components")+" (*.comp);;"+tr("All files")+" (*.*)" );
    if( fileName.isEmpty() ) return;

    save( fileName );
}


void ComposerWidget::settApp(){;}

void ComposerWidget::createActions()
{
    for( int i=0; i<MaxRecentFiles; i++ )
    {
        recentFileActs[i] = new QAction( this );
        recentFileActs[i]->setVisible( false );
        connect( recentFileActs[i], &QAction::triggered,
                 this,              &ComposerWidget::openRecentFile );
    }

    newAct = new QAction( QIcon(":/new.svg"), tr("New C&omponent\tCtrl+N"), this);
    newAct->setStatusTip( tr("Create a new Component"));
    connect( newAct, &QAction::triggered,
               this, &ComposerWidget::newCanvas );

    openAct = new QAction( QIcon(":/open.svg"), tr("&Open Component\tCtrl+O"), this);
    openAct->setStatusTip( tr("Open an existing Component"));
    connect( openAct, &QAction::triggered,
                this, &ComposerWidget::open );

    saveAct = new QAction( QIcon(":/save.svg"), tr("&Save Component\tCtrl+S"), this);
    saveAct->setStatusTip( tr("Save this Component to disk"));
    connect( saveAct, &QAction::triggered,
                this, QOverload<>::of(&ComposerWidget::save) );

    saveAsAct = new QAction( QIcon(":/saveas.svg"),tr("Save Component &As...\tCtrl+Shift+S"), this);
    saveAsAct->setStatusTip( tr("Save this Component under a new name"));
    connect( saveAsAct, &QAction::triggered,
                  this, &ComposerWidget::saveAs );

    zoomFitAct = new QAction( QIcon(":/zoomfit.svg"),tr("Zoom to fit"), this);
    zoomFitAct->setStatusTip( tr("Zoom Canvas to fit all components"));
    connect( zoomFitAct, &QAction::triggered,
            &m_composerView, &ComposerView::zoomToFit );

    zoomSelAct = new QAction( QIcon(":/zoomsel.svg"),tr("Zoom to selected"), this);
    zoomSelAct->setStatusTip( tr("Zoom Canvas to fit all selected components"));
    connect( zoomSelAct, &QAction::triggered,
            &m_composerView, &ComposerView::zoomSelected );

    zoomOneAct = new QAction( QIcon(":/zoomone.svg"),tr("Zoom to Scale 1"), this);
    zoomOneAct->setStatusTip( tr("Zoom Canvas to Scale 1:1"));
    connect( zoomOneAct, &QAction::triggered,
             &m_composerView, &ComposerView::zoomOne );

    settAppAct = new QAction( QIcon(":/config.svg"),tr("Settings"), this);
    settAppAct->setStatusTip(tr("Settings"));
    connect( settAppAct, &QAction::triggered,
                   this, &ComposerWidget::settApp );
}

void ComposerWidget::createToolBars()
{
    m_toolBar.setObjectName( "m_toolBar" );

    double fs = MainWindow::self()->fontScale()*20;
    m_toolBar.setIconSize( QSize( fs, fs ) );

    m_toolBar.addAction( settAppAct );
    QWidget* spacer = new QWidget();
    spacer->setFixedWidth( 15 );
    m_toolBar.addWidget( spacer );
    m_toolBar.addSeparator();//..........................

    for( int i=0; i<MaxRecentFiles; i++ ) m_fileMenu.addAction( recentFileActs[i] );
    QToolButton* fileButton = new QToolButton( this );
    fileButton->setToolTip( tr("Last Files") );
    fileButton->setMenu( &m_fileMenu );
    fileButton->setIcon( QIcon(":/lastfiles.svg") );
    fileButton->setPopupMode( QToolButton::InstantPopup );
    m_toolBar.addWidget( fileButton );

    m_toolBar.addAction( newAct );

    m_toolBar.addAction( openAct );
    m_toolBar.addAction( saveAct );
    m_toolBar.addAction( saveAsAct );
    spacer = new QWidget();
    spacer->setFixedWidth( 15 );
    m_toolBar.addWidget( spacer );
    m_toolBar.addSeparator();

    m_toolBar.addAction( zoomFitAct );
    m_toolBar.addAction( zoomSelAct );
    m_toolBar.addAction( zoomOneAct );
}

void ComposerWidget::updtRecentFiles()
{
    QSettings* settings = MainWindow::self()->settings();
    QStringList files = settings->value("Composer/recentFiles").toStringList();
    files.removeAll( m_currentFile );
    files.prepend( m_currentFile );
    while( files.size() > MaxRecentFiles ) files.removeLast();

    settings->setValue("Composer/recentFiles", files );
    updtRecentFileActions();
}

void ComposerWidget::updtRecentFileActions()
{
    QSettings* settings = MainWindow::self()->settings();
    QStringList files = settings->value("Composer/recentFiles").toStringList();

    int numRecentFiles = qMin( files.size(), (int)MaxRecentFiles );

    for( int i=0; i<numRecentFiles; i++ )
    {
        QString text = tr("&%1 %2").arg(i + 1).arg( getFileName( files[i] ) );
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData( files[i] );
        recentFileActs[i]->setVisible( true );
    }
    for( int i=numRecentFiles; i<MaxRecentFiles; i++ ) recentFileActs[i]->setVisible(false);
}
