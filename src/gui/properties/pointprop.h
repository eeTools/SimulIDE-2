/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "comproperty.h"

class PointProp : public ComProperty
{
    public:
        PointProp( CompBase* comp, param_t p, uint8_t idInt );
        ~PointProp();

        void    setValStr( QString valStr ) override;
        //QString getValStr() override;

    private:

};
