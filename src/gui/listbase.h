/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef LISTBASE_H
#define LISTBASE_H

#include <QDropEvent>
#include <QDir>
#include <QTreeWidget>

#include "compbase.h"

class TreeItem;
class QDomNode;

struct prop_t{
    QString name;
    QString value;
};

class ListBase : public QTreeWidget
{
    public:
        ListBase( QWidget* parent );
        ~ListBase();

        QString getDataFile( QString compName ) { return m_dataFileList.value( compName ); }
        QString getFileDir( QString compName ) { return m_dirFileList.value( compName ); }

        CompBase* createItem( QString type, int id );

        TreeItem* getCategory( QString category );
        QStringList getCategories(){ return m_categories.keys(); }

        void search( QString filter );

    private slots:
        void slotItemClicked( QTreeWidgetItem* item, int );


    protected:
        virtual void registerItems(){;}

        void addItem( listItem_t item );
        void addItem( QString label, TreeItem* catItem, QString icon, QString type );
        void addItem( QString label, TreeItem* catItem, QIcon &icon, QString type );
        TreeItem* addCategory( QString nameTr, QString name, QString parent, QString icon );

        QString getIcon( QString folder, QString name );

        bool m_customComp;

        QString m_searchFilter;

        QDir m_compSetDir;

        QHash<QString, TreeItem*> m_components;
        QHash<QString, TreeItem*> m_categories;
        QHash<QString, QString>   m_catNames;

        QHash<QString, QString> m_dataFileList;
        QHash<QString, QString> m_dirFileList;
        QHash<QString, QString> m_shortCuts;

        QMap<QString, listItem_t> m_componentFactory;
};
#endif
