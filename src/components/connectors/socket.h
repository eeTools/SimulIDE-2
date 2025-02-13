/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "connbase.h"

class Socket : public ConnBase
{
    public:
        Socket( int id );
        ~Socket();

 static listItem_t registerItem();

        void updateStep() override;

        void updateConnections( bool u  );

    private:
        void updatePixmap() override;
};
