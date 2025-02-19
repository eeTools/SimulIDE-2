/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "tra.h"
#include "component.h"

class BJT : public Component, public Bjt
{
    public:
        BJT( int id );
        ~BJT();

        void updateStep() override;

        void paint( QPainter* p, const QStyleOptionGraphicsItem*, QWidget* ) override;

 static listItem_t registerItem();
};
