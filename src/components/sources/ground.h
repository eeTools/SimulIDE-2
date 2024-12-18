/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef GROUND_H
#define GROUND_H

#include "component.h"
#include "pinsource.h"

class Ground : public Component
{
    public:
        Ground( QString id );
        ~Ground();

 static listItem_t registerItem();

        virtual void initialize() override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;

    private:
        PinSource m_out;
};

#endif
