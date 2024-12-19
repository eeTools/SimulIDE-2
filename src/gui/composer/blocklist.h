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

    private:
        void registerItems() override;

 static BlockList* m_pSelf;
};
#endif
