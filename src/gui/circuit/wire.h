/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef WIRE_H
#define WIRE_H

#include "route.h"

class WireLine;

class Wire : public Route
{
    friend class WireLine;

    public:
        Wire( QString id, PinBase* startpin, PinBase* endpin=nullptr );
        ~Wire();

        void setPointListStr( QString pl ) override;
        void setPointList( QStringList pl ) override;

        void updateConRoute( PinBase* pin ) override;
        void updateConRoute( QPointF thisPoint ) override;
        void closeCon( PinBase* endpin ) override;

        void setVisib( bool vis ) override;
        void select( bool selected ) override;

        void move( QPointF delta ) override;
        void remove() override;

        WireLine* addConLine( int x1, int y1, int x2, int y2, int index );

        double getVoltage();

    protected:
        void refreshPointList() override;
        void updateConRoute( PinBase* pin, QPointF thisPoint );
        void remConLine( WireLine* line  );
        void connectLines( int index1, int index2 );
        void disconnectLines( int index1, int index2 );
        void addConLine( WireLine* line, int index );
        void splitCon( int index, PinBase* pin0, PinBase* pin2 );
        QList<WireLine*>* lineList() { return &m_wireLineList; }
        void updateLines();
        void remNullLines();
        void remLines();

        QList<WireLine*> m_wireLineList;
};

#endif
