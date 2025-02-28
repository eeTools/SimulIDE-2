/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QString>
#include <QPointF>
#include <QMap>
#include <QCoreApplication>

#include "proputils.h"
#include "proptypes.h"

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
    uint8_t flags;
};

struct listItem_t{
    QString label;
    QString category;
    QString icon;
    QString type;
    CompBase* (*construct)(int);
};


class PropDialog;

class CompBase
{
    friend class PropDialog;
    friend class ListBase;

    public:
        CompBase( int id );
        virtual ~CompBase();

        int     getUid()   { return m_id; }
        QString itemType() { return m_type; }

        void addPropGroup( propGroup pg, bool list=true );
        //void addPropGroup( propGroup pg, const std::vector<param_t> &props, bool list=true );
        //void remPropGroup( QString name );
        //void addProp( propGroup& pg, param_t p );
        void addProperty( QString group, ComProperty* p );
        //void remProperty( QString prop );

        //uint8_t getPropertyId( QString name );
        QList<propGroup> getPropGroups() { return m_propGroups; }
        //QList<propGroup>* properties() { return &m_propGroups; } // Circuit needs pointer bcos properties can change (ex: loadHex changes Config)

        uint8_t newPropId() { return m_propCount++; }

        virtual void propertyChanged( const ComProperty* prop ){;}

        virtual void loadProperties( QVector<propStr_t> p );

        virtual bool    setPropStr( QString prop, QString val );
        virtual QString getPropStr( QString prop );

        virtual bool isHidden() { return false; }

        virtual void setup(){;} // Called after all properties are set

        virtual QString toString();

    protected:

        ComProperty* getProperty( QString name );

        int64_t m_id;
        QString m_type;
        QString m_help;

        uint8_t m_propCount;

        PropDialog* m_propDialog;
        QList<propGroup> m_propGroups;
        QMap<QString, ComProperty*> m_propMap;
        std::vector<ComProperty*> m_propVector;
};
