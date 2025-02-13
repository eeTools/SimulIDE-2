/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef ELEMENT_H
#define ELEMENT_H

#include <QString>
//#include <vector>

class Node;
class Kcl;

class Element
{
    public:
        Element();
        ~Element();

        //virtual void reset(){;}
        virtual void stampAdmit(){;}
        virtual void stampCurrent(){;}
        virtual void voltChanged(){;}

        //QString getId() { return m_elId; }

        Element* next;

 static constexpr double cero_doub = 1e-14;
 static constexpr double high_imp  = 1e14;

    protected:
        //QString m_elId;

        Kcl* m_kcl;
};

#endif
