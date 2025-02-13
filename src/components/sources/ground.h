/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "pinsource.h"

class IoPin;

class Ground : public Component
{
    public:
        Ground( int id );
        ~Ground();

 static listItem_t registerItem();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;

    private:
        IoPin* m_out;
};
