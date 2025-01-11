/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef GROUND_H
#define GROUND_H

#include "component.h"
#include "pinsource.h"

class IoPin;

class Ground : public Component
{
    public:
        Ground( QString id );
        ~Ground();

 static listItem_t registerItem();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;

    private:
        IoPin* m_out;
};

#endif
