/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BLOCKLIST_H
#define BLOCKLIST_H

#include <QTreeWidget>

#include "module.h"

class BlockList : public QTreeWidget
{
    public:
        BlockList( QWidget* parent );
        ~BlockList();

 static BlockList* self() { return m_pSelf; }

        Module* createModule( QString type, QString id );

        void addItem( moduleItem_t item );

    private slots:
        void slotItemClicked( QTreeWidgetItem* item, int );

    private:
        void registerItems();

    QMap<QString, Module* (*)(QString)> m_moduleFactory;

 static BlockList* m_pSelf;
};
#endif
