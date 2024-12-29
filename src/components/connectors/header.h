/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef HEADER_H
#define HEADER_H

#include "connbase.h"

class Header : public ConnBase
{
    public:
        Header( QString id );
        ~Header();

 static listItem_t registerItem();

        bool pointDown() { return m_pointDown; }
        void setPointDown( bool down );

        void updatePins() override;

    private:
        void updatePixmap() override;

        bool m_pointDown;
};

#endif
