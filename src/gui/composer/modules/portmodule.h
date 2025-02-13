/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "module.h"
#include "modsignal.h"

enum side_t{
    Right=0,
    Left,
    Top,
    Bottom
};

class PinBase;

class PortModule : public Module
{
    public:
        PortModule( int id );
        ~PortModule();

        enum portType_t{
            portIO=0,
            portHook
        };

        void setComponent( fComponent* c ) override;

        side_t sideTipe() { return m_side;}
        QString side() { return m_sideStr; }
        void setSide( QString s );

        int position() { return m_position; }
        void setPosition( int p );
        void updatePosition( double p );

        int size() { return m_size; }
        virtual void setSize( int s );

        int direction() { return m_direction; }
        void setDirection( int d ) { m_direction = d; }

        QString pinNames() { return m_pinNames; }
        void setPinNames( QString n );

        QList<PinBase*> getPinList() { return m_pins; }

        portType_t portType() { return m_portType; }

        void remove() override;

    protected:
        virtual PinBase* addPin( QString id ){ return nullptr;}

        int m_size;
        int m_position;
        int m_direction;

        side_t m_side;
        QString m_sideStr;
        QStringList m_sideList;

        int m_state;

        portType_t m_portType;

        ModSlot   m_inputSlot;
        ModSignal m_outSignal;

        QString m_pinNames;

        QList<PinBase*> m_pins;
};
