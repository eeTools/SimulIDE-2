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
        DoubProp( CompBase* comp, QString idStr, QString label, QString unit, double val, uint8_t flags );
        ~DoubProp();

        void    setValStr( QString valStr ) override;
        QString getValStr() override;

        QString toString() override;

        // Component interface
        double get();
        void set( double val );

    private:

        QString m_multStr;
};
