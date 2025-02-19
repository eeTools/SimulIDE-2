/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "comproperty.h"

class DoubProp : public ComProperty
{
    friend class NumWidget;

    public:
        DoubProp( CompBase* comp, param_t p, uint8_t idInt );
        ~DoubProp();

        void    setValStr( QString valStr ) override;
        QString getValStr() override;

    private:
        double m_defaultVal;

        QString m_multStr;
};
