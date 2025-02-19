/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "comproperty.h"

class IntProp : public ComProperty
{
    public:
        IntProp( CompBase* comp, param_t p, uint8_t idInt );
        ~IntProp();

        void    setValStr( QString valStr ) override;
        //QString getValStr() override;

    private:
        int64_t m_defaultVal;

};
