/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef COMPROPERTY_H
#define COMPROPERTY_H

#include <QString>
#include <QList>

enum propFlags{
    propHidden = 1,
    propNoCopy = 1<<1,
    propSignal = 1<<2,
    propSlot   = 1<<3,
};

class PropWidget;

class ComProperty
{
    public:
        ComProperty( QString id, QString label, QString unit, QString type, uint8_t flags );
        virtual ~ComProperty(){;}

        QString id();
        QString label();
        QString type();
        QString unit();
        uint8_t flags();

        void setName( QString n );

        virtual void    setValStr( QString );
        virtual QString getValStr();
        virtual double  getValue();

        virtual QString toString();

        void addCallBack( ComProperty* cb );

        virtual PropWidget* getWidget();
        virtual PropWidget* createWidget();

    protected:

        QList<ComProperty*> m_callbacks;

        PropWidget* m_widget;

        QString m_id;
        QString m_label;
        QString m_type;
        QString m_unit;
        uint8_t m_flags;
};

#endif
