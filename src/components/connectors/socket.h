/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef SOCKET_H
#define SOCKET_H

#include "connbase.h"

class Socket : public ConnBase
{
    public:
        Socket( QString id );
        ~Socket();

 static listItem_t registerItem();

        void updateStep() override;

        void updateConnections( bool u  );

    private:
        void updatePixmap() override;
};

#endif
