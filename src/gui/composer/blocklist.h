/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BLOCKLIST_H
#define BLOCKLIST_H

#include <QTreeWidget>

#include "listbase.h"

class BlockList : public ListBase
{
    public:
        BlockList( QWidget* parent );
        ~BlockList();

 static BlockList* self() { return m_pSelf; }

        ///CompBase* createItem( QString type, QString id );

        /// void addItem( listItem_t item );

    private slots:
        //void slotItemClicked( QTreeWidgetItem* item, int );

    private:
        void registerItems() override;

    ////QMap<QString, Module* (*)(QString)> m_moduleFactory;

 static BlockList* m_pSelf;
};
#endif
