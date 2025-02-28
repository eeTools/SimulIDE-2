/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "comproperty.h"

class IntProp : public ComProperty
{
    public:
        IntProp( CompBase* comp, QString idStr, QString label, int64_t val, uint8_t flags );
        ~IntProp();

        void setValStr( QString valStr ) override;
        QString getValStr() override;

    private:

};
