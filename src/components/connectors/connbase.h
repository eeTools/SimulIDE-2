/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "element.h"
#include "pin.h"

class ConnBase : public Component, public Element
{
    public:
        ConnBase( int id );
        ~ConnBase();

        int  size() { return m_size; }
        void setSize( int size );

        void registerEnode( int, int n=-1 ) override;

        void setHidden( bool hid, bool hidArea=false, bool hidLabel=false ) override;

        void createPins( int c );
        void deletePins( int d );

        virtual void updatePins(){;}

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        virtual void updatePixmap(){;}

        int m_size;

        std::vector<Pin*> m_connPins;
        std::vector<Pin*> m_sockPins;

        Pin::pinType_t m_pinType;

        QPixmap m_pinPixmap;
};
