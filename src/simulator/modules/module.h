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

class Module : public CompBase
{
    friend class fComponent;

    public:
        Module( QString id );
        ~Module();

        //virtual void setProperty( QString name, QString val ) {;}

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
        virtual void connect( QString signal, ModSlot* slot, bool* modChanged, bool* fCompChanged  );

    protected:
        int m_index;

        bool m_changed;

        fComponent* m_component;
        FuncBlock*  m_funcBlock;

        std::vector<ModSignal*> m_signals;
        std::vector<ModSlot*>   m_slots;
};


struct moduleItem_t{
    QString label;
    QString type;
    QString category;
    QString icon;
    Module* (*construct)(QString);
};
#endif