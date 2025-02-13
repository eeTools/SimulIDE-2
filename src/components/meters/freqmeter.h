/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "element.h"

class FreqMeter : public Component, public Element
{
    public:
        FreqMeter( int id );
        ~FreqMeter();

 static listItem_t registerItem();

        void initialize() override;
        ///void stamp() override;
        void updateStep() override;
        void voltChanged() override;

        double filter() { return m_filter; }
        void setFilter( double f ) { m_filter = f; }
        
        void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;
        
    protected:
        void setflip() override;

        bool m_rising;
        bool m_falling;
        
        double m_filter;
        double m_lastData;
        double m_freq;
        
        int m_numMax;

        uint64_t m_lastMax;
        uint64_t m_totalP;
        uint64_t m_period;
        
        QGraphicsSimpleTextItem m_display;
};
