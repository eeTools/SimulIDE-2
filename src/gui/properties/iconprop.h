/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "comproperty.h"

class IconProp : public ComProperty
{
public:
    IconProp( CompBase* comp, param_t p, uint8_t idInt );
    ~IconProp();

    void    setValStr( QString valStr ) override;
    //QString getValStr() override;

private:
    bool m_defaultVal;

};
