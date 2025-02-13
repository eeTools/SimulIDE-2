/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

class fComponent;

class fModule
{
    public:
        fModule();
        ~fModule();

        virtual void connectSignal( fModule* signalModule, int slotId ){;}
        virtual void initModule(){;}
        virtual void remove(){;}

        void runStep(){
            if( !m_modChanged ) return;
            m_modChanged = false;
            updateOutput();
        }

        void addSlotModule( fModule* module );

        int* getOutputINT() { return &m_outputINT; }

        inline void changed() { m_modChanged = true; }

    protected:
        virtual void updateOutput(){;}
        void outputChanged();

        bool m_modChanged;

        int    m_outputINT;
        double m_outputDBL;

        //fComponent* m_component;
        bool* m_fCompChanged;

        fModule* m_slotModules;
        fModule* m_next;
};
