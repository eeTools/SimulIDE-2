/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#pragma once

#include "module.h"
#include "updatable.h"
#include "modsignal.h"

class CustomButton;
class QGraphicsProxyWidget;

class mButton : public Module, public Updatable
{
    public:
        mButton( int id );
        ~mButton();

 static listItem_t registerItem();

        void setComponent( fComponent* c ) override;
        void initModule() override;
        void updateStep() override;

        int width() { return m_width; }
        void setWidth( int width );

        int height() { return m_height; }
        void setHeight( int height );

        QPointF position() { return m_position; }
        void setPosition( QPointF p);

        void onbuttonclicked();

    private:
        void updateProxy();

        int m_output;
        ModSignal m_outSignal;

        int m_width;
        int m_height;

        QPoint m_position;
        QString m_key;

        CustomButton* m_button;
        QGraphicsProxyWidget* m_proxy;
};
