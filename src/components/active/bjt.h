/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BJT_H
#define BJT_H

#include "tra.h"
#include "component.h"

class BJT : public Component, public Bjt
{
    public:
        BJT( QString id );
        ~BJT();

 static listItem_t libraryItem();

        void updateStep() override;

        void paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* ) override;
};

#endif
