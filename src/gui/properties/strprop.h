/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "comproperty.h"

class StrProp : public ComProperty
{
    public:
        StrProp( CompBase* comp, param_t p, uint8_t idInt );
        ~StrProp();

        void    setValStr( QString valStr ) override;
        //QString getValStr() override;

    private:
        QString m_defaultVal;

};
