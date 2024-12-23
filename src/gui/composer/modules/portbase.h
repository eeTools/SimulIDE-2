/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef PORTBASE_H
#define PORTBASE_H

#include "module.h"
#include "modsignal.h"

enum side_t{
    Right=0,
    Left,
    Top,
    Bottom
};

class PinBase;

class PortBase : public Module
{
    public:
        PortBase( QString name );
        ~PortBase();

        void setComponent( fComponent* c ) override;

        side_t sideTipe() { return m_side;}
        QString side() { return m_sideStr; }
        void setSide( QString s );

        int position() { return m_position; }
        void setPosition( int p );
        void updatePosition( double p );

        int size() { return m_size; }
        void setSize( int s );

        int direction() { return m_direction; }
        void setDirection( int d ) { m_direction = d; }

        QString pinNames() { return m_pinNames; }
        void setPinNames( QString n );

    protected:
        virtual PinBase* addPin( QString id ){ return nullptr;}

        int m_size;
        int m_position;
        int m_direction;

        side_t m_side;
        QString m_sideStr;
        QStringList m_sideList;

        int m_state;

        ModSlot   m_inputSlot;
        ModSignal m_outSignal;

        QString m_pinNames;

        QList<PinBase*> m_pins;
};

#endif
