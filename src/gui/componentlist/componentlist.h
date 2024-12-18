/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef COMPONENTLIST_H
#define COMPONENTLIST_H

#include "listbase.h"
#include "managecomps.h"
#include "component.h"

class TreeItem;
class QDomNode;

class ComponentList : public ListBase
{
    public:
        ComponentList( QWidget* parent );
        ~ComponentList();

 static ComponentList* self() { return m_pSelf; }

        void LoadCompSetAt( QDir compSetDir );

        void setShortcut( QString s, QString c ) { m_shortCuts[s] = c; }
        QString getComponent( QString shortcut ) { return m_shortCuts.value( shortcut ); }

        void loadXml( QString xmlFile );

        void writeSettings();

        void mousePressEvent( QMouseEvent* event ) override;

        void dropEvent( QDropEvent* event ) override;

    private slots:
        void slotContextMenu( const QPoint& );
        void slotManageComponents();

    private:
 static ComponentList* m_pSelf;

        void registerItems() override;

        void loadComps( QDir compSetDir );
        void readConfig();
        void readNodCfg( QDomNode* node, TreeItem* parent );


        bool m_oldConfig;
        bool m_restoreList;
        QString m_listFile;

        manCompDialog m_mcDialog;
};

#endif
