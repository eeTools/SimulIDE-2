/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef MODULE_H
#define MODULE_H

#include <QString>
#include <vector>

#include "compbase.h"

class ModSlot;
class ModSignal;
class ModProp;
class fComponent;
class FuncBlock;
class Module;

class Module : public CompBase
{
    friend class fComponent;

    public:
        Module( int id );
        ~Module();

        virtual void setComponent( fComponent* c ) { m_component = c; }
        void setFuncBlock( FuncBlock*  f ) { m_funcBlock = f; }

        virtual void setup(){;}
        virtual void initModule(){;}
        virtual void runStep(){;}

        int index() { return m_index; }
        void setIndex( int i ) { m_index = i; }

        std::vector<ModSignal*> getSignals() { return m_signals; }
        std::vector<ModSlot*>   getSlots()   { return m_slots; }

        virtual ModSlot* getSlot( QString name );
        virtual void connect( QString signal, ModSlot* slot, bool* modChanged, bool* fCompChanged );

        virtual void remove(){;}

    protected:
        int m_index;

        bool m_modChanged;

        fComponent* m_component;
        FuncBlock*  m_funcBlock;

        std::vector<ModSignal*> m_signals;
        std::vector<ModSlot*>   m_slots;
};

#endif
