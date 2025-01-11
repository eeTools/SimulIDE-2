/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef PINSOURCE_H
#define PINSOURCE_H

#include "element.h"
#include "kcl.h"

class PinSource : public Element
{
    public:
        PinSource( QString id, int* node );
        ~PinSource();

        void stampAdmit() override;
        void stampCurrent() override;

        //void setNode( int n ) { m_node = n; }
        int getNode()  { return *m_node; }

        void setAdmitance( double a ) { m_admitance = a; }
        inline void updtAdmitance( double a )
        {
            if( m_admitance == a ) return;
            m_admitance = a;
            if( *m_node < 0 ) return;
            m_kcl->admitChanged( *m_node, -1, m_nodeGroup );
        }

        void setCurrent( double c ) { m_current = c; }
        inline void updateCurrent( double c )
        {
            if( m_current == c ) return;
            m_current = c;
            if( *m_node < 0 ) return;
            *m_currGroupChg = true;
            *m_currChanged  = true;
        }

        inline void updateVoltage( double v ) { updateCurrent( v*m_admitance ); }

    protected:

        double m_admitance;
        double m_current;

        int* m_node;
        int m_nodeGroup;

        int*  m_currGroupChg;
        bool* m_currChanged;
};

#endif
