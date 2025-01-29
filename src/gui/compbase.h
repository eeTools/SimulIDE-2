/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef COMPBASE_H
#define COMPBASE_H

#include <QString>
#include <QPointF>
#include <QMap>
#include <QCoreApplication>

#include "proputils.h"

#define simulideTr(comp_name,str) QCoreApplication::translate(comp_name,str)

class ComProperty;
class CompBase;

enum groupFlags{
    groupHidden = 1,
    groupNoCopy = 1<<1,
    groupPkg    = 1<<2
};

struct propGroup{
    QString name;
    QList<ComProperty*> propList;
    int flags;
};

struct listItem_t{
    QString label;
    QString category;
    QString icon;
    QString type;
    CompBase* (*construct)(QString);
};

class PropDialog;

class CompBase
{
    friend class PropDialog;

    public:
        CompBase( QString id );
        virtual ~CompBase();

        void addPropGroup( propGroup pg, bool list=true );
        void remPropGroup( QString name );
        void addProperty( QString group, ComProperty* p );
        void remProperty( QString prop );
        ComProperty* getProperty( QString name );
        QList<propGroup> getPropGroups() { return m_propGroups; }
        //QList<propGroup>* properties() { return &m_propGroups; } // Circuit needs pointer bcos properties can change (ex: loadHex changes Config)

        virtual void loadProperties( QVector<propStr_t> p );

        virtual bool    setPropStr( QString prop, QString val );
        virtual QString getPropStr( QString prop );

        virtual QString toString();

        QString getUid() { return m_id; }
        void setUid( QString uid ) { m_id = uid; }

        QString itemType()  { return m_type; }
        void setItemType( QString t ) { m_type = t; }

        virtual bool isHidden() { return false;}

        virtual void setup(){;} // Called after all properties are set

    protected:
        QString m_id;
        QString m_type;
        QString m_help;

        PropDialog* m_propDialog;
        QList<propGroup> m_propGroups;
        QMap<QString, ComProperty*> m_propMap;
};

#endif
