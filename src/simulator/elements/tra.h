/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BJTE_H
#define BJTE_H

#include "element.h"

class Bjt : public Element
{
    public:

        Bjt();
        virtual ~Bjt();

        void stampAdmit() override;
        void voltChanged() override;

    protected:

        bool m_PNP;

};

#endif
