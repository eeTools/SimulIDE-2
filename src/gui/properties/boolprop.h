/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "comproperty.h"

class BoolProp : public ComProperty
{
    public:
        BoolProp( CompBase* comp, param_t p, uint8_t idInt );
        ~BoolProp();

        void    setValStr( QString valStr ) override;
        //QString getValStr() override;

    private:
        bool m_defaultVal;

};
