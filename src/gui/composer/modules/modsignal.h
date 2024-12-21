 /***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef MODSIGNAL_H
#define MODSIGNAL_H

#include <vector>
#include <QString>

#include "hooktype.h"

//----------------------  SIGNAL  --------------------------------
class ModSlot;

class ModSignal
{
    public:
        ModSignal( QString name, hookType_t type );
        ~ModSignal();

        QString    name() { return m_name; }
        hookType_t type() { return m_type; }

        void connect( ModSlot* slot, bool* modChanged, bool* fCompChanged );
        //void disconnect( ModSlot* slot );

        void changed();

        void setIntData( int* intData ) { m_intData = intData; }
        void setDblData( double* dblData ) { m_dblData = dblData; }

    private:
        QString m_name;

        bool* m_fCompChanged;
        std::vector<bool*> m_moduleChanged;

        int*    m_intData;
        double* m_dblData;

        hookType_t m_type;
};

//----------------------  SLOT  --------------------------------

class ModSlot
{
        friend class ModSignal;

    public:
        ModSlot( QString name, hookType_t type/*, bool* changed*/ );
        ~ModSlot();

        QString name() { return m_name; }
        hookType_t type() { return m_type; }

        void clear();
        int intData() { return *m_signalInt; }

        //virtual void setValue( double ){;}
        //virtual void setValue( int ){;}
        //virtual void setValue( int, int ){;}

        //virtual double getValue(){ return 0; }
        //virtual int getValue(){return 0;}

        //void addCallBack( ModSlot* cb ) { m_callBack = cb; }

    protected:
        QString m_name;

        hookType_t m_type;

        //bool*   m_signalChg;
        int*    m_signalInt;
        double* m_signalDbl;

        ModSlot* nextSlot;
        //ModSlot* m_callBack;
};

#endif
