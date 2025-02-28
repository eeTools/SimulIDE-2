/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "comproperty.h"

class StrProp : public ComProperty
{
    public:
        StrProp( CompBase* comp, QString idStr, QString label, QString val, uint8_t flags );
        ~StrProp();

        void    setValStr( QString valStr ) override;
        QString getValStr() override;

    private:
        QString m_strVal;

};
