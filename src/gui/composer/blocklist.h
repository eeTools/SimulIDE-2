/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BLOCKLIST_H
#define BLOCKLIST_H

#include <QTreeWidget>

#include "listbase.h"

class fModule;
class fComponent;

class BlockList : public ListBase
{
    public:
        BlockList( QWidget* parent );
        ~BlockList();

        fModule* createModule( QString type, QString uid, fComponent* fComp, QList<prop_t> properties );

 static BlockList* self() { return m_pSelf; }

    private:
        void registerItems() override;

 static BlockList* m_pSelf;
};
#endif
