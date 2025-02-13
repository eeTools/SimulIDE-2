/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "plotbase.h"

class LaChannel;
class LaWidget;
class DataLaWidget;

struct sample_t{
    double value;
    uint channel;
};

class LAnalizer : public PlotBase
{
    public:
        LAnalizer( int id );
        ~LAnalizer();

 static listItem_t registerItem();

        void updateStep() override;

        QString timPos() override;
        void setTimPos( QString tp ) override;

        QString volDiv() override;
        void setVolDiv( QString vd ) override;

        void setTimeDiv( uint64_t td ) override;

        int64_t timePos(){ return m_timePos; }
        void setTimePos( int64_t tp );
        void moveTimePos( int64_t delta ) override;

        double voltDiv(){ return m_voltDiv; }
        void setVoltDiv( double vd );

        void setIsBus( int ch, bool b ) { m_channel[ch]->setIsBus( b ); }

        void setTrigger( int ch ) override;

        double thresholdR() { return m_thresholdR; }
        void setThresholdR( double thr );

        double thresholdF() { return m_thresholdF; }
        void setThresholdF( double thr );

        QString busStr();
        void setBusStr( QString b );

        void setConds( QString conds ) override;

        void setTunnels( QString tunnels ) override;

        void expand( bool e ) override;

        void dumpData( const QString& fn ) override;

    private:
        uint64_t getGcd( uint64_t a, uint64_t b ); // greatest Common Denominator

        double m_voltDiv;
        double m_thresholdR;
        double m_thresholdF;

        int m_updtCount;

        int64_t m_timePos;

        LaWidget*  m_laWidget;
        DataLaWidget* m_dataWidget;
};
