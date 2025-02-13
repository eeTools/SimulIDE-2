/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef WIREBASE_H
#define WIREBASE_H

#include "compbase.h"

enum wireFlags_t{
    wireFunc = 1<<0,
    wireBus  = 1<<1
};

class PinBase;

class WireBase : public CompBase
{
    public:
        WireBase( int id, PinBase* startpin );
        ~WireBase();

        void dummySetter( QString ) {;}

        QString startPinId();
        QString endPinId();

        PinBase* startPin() { return m_startPin;}
        void setStartPin( PinBase* pin ) { m_startPin = pin; }

        PinBase* endPin() { return m_endPin; }
        void setEndPin( PinBase* pin) { m_endPin = pin; }

        void incActLine() { if( m_actLine < m_pList.size()-1 ) m_actLine += 1; }

        QStringList pointList() { refreshPointList(); return m_pointList; }
        QString pListStr() { refreshPointList(); return m_pointList.join(","); }
        virtual void setPointListStr( QString pl ){;}
        virtual void setPointList( QStringList pl ){;}

        virtual void closeCon( PinBase* endpin ){;}

        virtual void updateConRoute( PinBase* pin ){;}
        virtual void updateConRoute( QPointF thisPoint ){;}
        virtual void move( QPointF delta ){;}
        virtual void remove(){;}

        virtual void select( bool s ) {;}
        virtual void setVisib( bool vis ){;}

        void writeWireFlag( int flag, bool val );
        void setWireFlags( int flags ) { m_wireFlags = flags; }
        int  wireFlags() { return m_wireFlags; }

        bool m_freeLine;

    protected:
        virtual void refreshPointList(){;}

        int m_actLine;
        int m_lastIndex;
        int m_wireFlags;

        PinBase* m_startPin;
        PinBase* m_endPin;

        QStringList m_pointList;
        QList<QPoint> m_pList;
};
#endif
