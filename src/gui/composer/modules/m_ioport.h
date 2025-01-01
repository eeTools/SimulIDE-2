/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef MIOPORT_H
#define MIOPORT_H

#include "portbase.h"
#include "ioport.h"
#include "logicfamily.h"

class mIoPort : public PortBase, public IoPort, public LogicFamily
{
    public:
        mIoPort( QString name );
        virtual ~mIoPort();

 static listItem_t registerItem();

        void setup() override;

        void initModule() override;
        void runStep() override;

        QList<ComProperty*> inputProps();
        QList<ComProperty*> outputProps();
        //QList<ComProperty*> outputType();
        QList<ComProperty*> edgeProps();

        void setInpHighV( double volt ) override;
        void setInpLowV( double volt ) override;
        void setOutHighV( double volt ) override;
        void setOutLowV( double volt ) override;
        void setInputImp( double imp ) override;
        void setOutImp( double imp ) override;
        void setRiseTime( uint64_t time ) override;
        void setFallTime( uint64_t time ) override;

        //bool openCol() { return m_openCol; }
        //void setOpenCol( bool op );

        void setSize( int size ) override;

    protected:
        PinBase* addPin( QString id ) override;

};
#endif
