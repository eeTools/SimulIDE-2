/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef EPIN_H
#define EPIN_H

#include <QString>

class Pin;

class ePin
{
    public:
        ePin( QString id );
        virtual ~ePin();

        bool isConnected() { return (m_enode != -1); }

        virtual double getVoltage();

        int getNode()  { return m_enode; }

        bool inverted() { return m_inverted; }
        void setInverted( bool i ) { m_inverted = i; }

        void setId( QString id ) { m_id = id; }
    protected:
        int m_enode;

        QString m_id;

        bool m_inverted;
};

#endif
