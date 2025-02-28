/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "comproperty.h"

class BoolProp : public ComProperty
{
    public:
        BoolProp( CompBase* comp, QString idStr, QString label, bool val, uint8_t flags );
        ~BoolProp();

        // External interface
        void setValStr( QString valStr ) override;
        QString getValStr() override;

        // Component interface
        bool get();
        void set( bool val );

};
