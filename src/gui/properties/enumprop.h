/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "comproperty.h"

class EnumProp : public ComProperty
{
public:
    EnumProp( CompBase* comp, param_t p, uint8_t idInt );
    ~EnumProp();

    void    setValStr( QString valStr ) override;
    //QString getValStr() override;

private:
    QString m_defaultVal;

};
