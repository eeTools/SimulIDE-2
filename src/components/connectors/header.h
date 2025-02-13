/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "connbase.h"

class Header : public ConnBase
{
    public:
        Header(int id );
        ~Header();

 static listItem_t registerItem();

        bool pointDown() { return m_pointDown; }
        void setPointDown( bool down );

        void updatePins() override;

    private:
        void updatePixmap() override;

        bool m_pointDown;
};
