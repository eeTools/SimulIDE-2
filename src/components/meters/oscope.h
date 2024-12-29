/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef OSCOPE_H
#define OSCOPE_H

#include "plotbase.h"

class OscopeChannel;
class OscWidget;
class DataWidget;

class Oscope : public PlotBase
{
    public:
        Oscope( QString id );
        ~Oscope();

 static listItem_t registerItem();

        double filter() { return m_filter; }
        void setFilter( double filter );

        int autoSC() { return m_auto; }
        void setAutoSC( int ch );

        QString hideCh();
        void setHideCh( QString hc );

        int tracks();
        void setTracks( int tracks );

        QString timPos() override;
        void setTimPos( QString tp ) override;

        QString volDiv() override;
        void setVolDiv( QString vd ) override;

        QString volPos();
        void setVolPos( QString vp );

        void setTrigger( int ch ) override;

        void updateStep() override;

        void setTimeDiv( uint64_t td ) override;

        int64_t timePos( int ch ){ return m_timePos[ch]; }
        void setTimePos( int ch, int64_t tp );
        void moveTimePos( int64_t delta ) override;

        double voltDiv( int ch ){ return m_voltDiv[ch]; }
        void setVoltDiv( int ch, double vd );

        double voltPos( int ch ){ return m_voltPos[ch]; }
        void setVoltPos( int ch, double vp );

        void hideChannel( int ch, bool hide );

        void setTunnels( QString tunnels ) override;

        void expand( bool e ) override;

        DataWidget* dataW() { return m_dataWidget; }

    private:
        double m_filter;

        int m_auto;

        int64_t m_timePos[4];
        double  m_voltDiv[4];
        double  m_voltPos[4];
        bool    m_hideCh[4];

        OscWidget*  m_oscWidget;
        DataWidget* m_dataWidget;
};

#endif
