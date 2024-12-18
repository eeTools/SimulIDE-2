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
        ModSignal( QString name, hookType_t type/*, bool* changed*/ );
        ~ModSignal();

        QString    name() { return m_name; }
        hookType_t type() { return m_type; }

        void connect( ModSlot* slot, bool* modChanged, bool* fCompChanged );
        //void disconnect( ModSlot* slot );

        void changed();

        void setIntData( int* intData ) { m_intData = intData; }
        void setDblData( double* dblData ) { m_dblData = dblData; }

        //void changed( bool chg ) { m_changed = chg; }
        //void send( double val ); // Calls all connected slots.
        //void send( int val ); // Calls all connected slots.

    private:
        QString m_name;

        bool* m_fCompChanged;
        std::vector<bool*> m_moduleChanged;

        int*    m_intData;
        double* m_dblData;

        hookType_t m_type;

        /// ModSlot* m_slot;
};

//----------------------  SLOT  --------------------------------

/*class ModSlot
{
        friend class ModSignal;

    public:
        ModSlot( QString name, hookType_t type )
        {
            m_name = name;
            m_type = type;
            nextSlot = nullptr;
            m_callBack = nullptr;
            //m_bits   = 1;
        }
        virtual ~ModSlot(){;}

        QString name() { return m_name; }
        hookType_t type() { return m_type; }

        virtual void setValue( double ){;}
        virtual void setValue( int ){;}
        //virtual void setValue( int, int ){;}


        virtual double getValue(){ return 0; }
        //virtual int getValue(){return 0;}

        void addCallBack( ModSlot* cb ) { m_callBack = cb; }

        //int bits() { return m_bits; }

    protected:
        QString m_name;

        hookType_t m_type;

        //int m_bits;

        ModSlot* nextSlot;
        ModSlot* m_callBack;
};*/

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

/*template <class Obj>
class SetBitSlot : public ModSlot
{
    public:
        SetBitSlot( QString name, Obj* object, void (Obj::*func)(int, int), int bit )
        : ModSlot( name, hookInputBit )
        {
            m_object = object;
            m_func   = func;
            m_bit    = bit;
        }
        ~SetBitSlot(){;}

        void setSize( int b )
        {
            m_bits = b;
            if( m_callBack ) m_callBack->setValue( b );
        }

        virtual void setValue( int val ) override
        { (m_object->*m_func)(val, m_bit); }

    private:
        int m_bit;
        Obj* m_object;
        void (Obj::*m_func)(int, int);
};*/

/*template <class Obj>
class SetIntSlot : public ModSlot
{
    public:
        SetIntSlot( QString name, bool* changed )
        : ModSlot( name, hookInputInt, changed )
        {
        }
        ~SetIntSlot(){;}

        void setSignalData( int* data )
        { m_signalData = data; }

    private:
        int* m_signalData;
};*/

/*template <class Obj>
class SetIntSlot : public ModSlot
{
    public:
        SetIntSlot( QString name, Obj* object, void (Obj::*func)(int) )
        : ModSlot( name, hookInputInt )
        {
            m_object = object;
            m_func   = func;
            nextSlot = nullptr;
        }
        ~SetIntSlot(){;}

        virtual void setValue( int val ) override
        { (m_object->*m_func)(val); }

        //virtual void setValue( int val, int mask=-1 ) override
        //{ (m_object->*m_func)(val, mask); }

    private:
        Obj* m_object;
        void (Obj::*m_func)(int);
        //void (Obj::*m_func)(int, int);
};*/

/*template <class Obj>
class GetIntSlot : public ModSlot
{
    public:
        GetIntSlot( QString name, Obj* object, int (Obj::*func)() )
        : ModSlot( name, hookOutputInt )
        {
            m_object = object;
            m_func   = func;
            nextSlot = nullptr;
        }
        ~GetIntSlot(){;}

        virtual double getValue() override
        { return (m_object->*m_func)(); }

    private:
        Obj* m_object;
        int (Obj::*m_func)();
};*/

/*template <class Obj>
class SetDoubSlot : public ModSlot
{
    public:
        SetDoubSlot( QString name, Obj* object, void (Obj::*func)(double) )
        : ModSlot( name, hookInputDoub )
        {
            m_object = object;
            m_func   = func;
            nextSlot = nullptr;
        }
        ~SetDoubSlot(){;}

        virtual void setValue( double val ) override
        { (m_object->*m_func)(val); }

    private:
        Obj* m_object;
        void (Obj::*m_func)(double);
};

/*template <class Obj>
class GetDoubSlot : public ModSlot
{
    public:
        GetDoubSlot( QString name, Obj* object, double (Obj::*func)() )
        : ModSlot( name, hookOutputDoub )
        {
            m_object = object;
            m_func   = func;
            nextSlot = nullptr;
        }
        ~GetDoubSlot(){;}

        virtual double getValue() override
        { return (m_object->*m_func)(); }

    private:
        Obj* m_object;
        double (Obj::*m_func)();
};*/
#endif
