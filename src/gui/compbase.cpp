/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "compbase.h"
#include "circuit.h"
#include "propdialog.h"
#include "boolprop.h"
#include "doubprop.h"
#include "intprop.h"
//#include "iconprop.h"
#include "pointprop.h"
//#include "enumprop.h"
#include "strprop.h"

CompBase::CompBase( int id )
{
    m_id = id;

    m_propDialog = nullptr;

    /*addPropGroup( {"CompBase", {}, groupHidden | groupNoCopy },
    {
        //new StrProp <Component>("itemtype","","", this, &Component::itemType,  &Component::setItemType ),
        {"uid","","", &m_id, P_Int, 0 }
    });*/
}
CompBase::~CompBase()
{
    if( m_propDialog )
    {
        m_propDialog->setParent( nullptr );
        m_propDialog->close();
        delete m_propDialog;
    }
}

void CompBase::loadProperties( QVector<propStr_t> properties ) // Set properties in correct order
{
    for( propStr_t prop : properties ) // Set properties
        setPropStr( prop.name.toString(), prop.value.toString() );
}

/*void CompBase::remPropGroup( QString name )
{
    for( int i=0; i<m_propGroups.size(); ++i )
    {
        if( m_propGroups.at(i).name == name )
        {
            for( ComProperty* p : m_propGroups.at(i).propList ) m_propMap.remove( p->id() );
            m_propGroups.removeAt(i);
            break;
}   }   }*/

/*void CompBase::addPropGroup( propGroup pg, const std::vector<param_t> &props, bool list )
{
    for( param_t param : props )
    {
        ComProperty* cp;

        switch( param.type )
        {
        case P_Uint:
        case P_Int:    cp = new IntProp( this, param, newPropId() );  break;
        case P_Double: cp = new DoubProp( this, param, newPropId() ); break;
        case P_Bool:   cp = new BoolProp( this, param, newPropId() ); break;
        case P_Icon:   cp = new IconProp( this, param, newPropId() ); break;
        case P_Enum:   cp = new EnumProp( this, param, newPropId() ); break;
        case P_Point:  cp = new PointProp( this, param, newPropId() ); break;
        case P_String: cp = new StrProp( this, param, newPropId() );  break;
        }
        if( !cp ) continue;

        m_propVector.emplace_back( cp );
        pg.propList.append( cp );
        if( list ) m_propMap[cp->idStr()] = cp;
    }
    m_propGroups.append( pg );
}*/

/*void CompBase::addProp( propGroup& pg, param_t p )
{

}*/

void CompBase::addPropGroup( propGroup pg, bool list )
{
    m_propGroups.append( pg );

    if( list )
        for( ComProperty* p : pg.propList ) m_propMap[p->label()] = p;
}

void CompBase::addProperty( QString group, ComProperty* p )
{
    for( int i=0; i<m_propGroups.size(); ++i )
    {
        propGroup pg = m_propGroups.at(i);
        if( pg.name != group ) continue;

        pg.propList.append( p );
        m_propGroups.replace( i, pg );
        m_propMap[p->idStr()] = p;
        return;
}   }

/*void CompBase::remProperty( QString prop )
{
    for( int i=0; i<m_propGroups.size(); ++i )
    {
        propGroup pg = m_propGroups.at(i);
        for( ComProperty* p : pg.propList )
        {
            if( p->id() != prop ) continue;
            pg.propList.removeAll( p );
            m_propGroups.replace( i, pg );
            m_propMap.remove( prop );
            delete p;
            return;
}   }   }*/

/*uint8_t CompBase::getPropertyId( QString name )
{
    return getProperty( name )->idInt();
}*/

ComProperty* CompBase::getProperty( QString id )
{
    return m_propMap.value( id );
}

bool CompBase::setPropStr( QString prop, QString val )
{
    ComProperty* p = m_propMap.value( prop );
    if( p ){
        p->setValStr( val );
        if( m_propDialog ) m_propDialog->updtValues();
    }
    else return false;
    return true;
}
QString CompBase::getPropStr( QString prop )
{
    ComProperty* p = m_propMap.value( prop );
    if( p ) return p->getValStr();
    return "";
}

QString CompBase::toString() // Used to save circuit
{
    QString item = "\n"+m_type;
    /*for( propGroup pg : m_propGroups )
    {
        if( !Circuit::self()->getBoard() )     // Not a Subcircit Board
        {
            if( pg.name == "Board") continue;  // Don't save Board properties
        }

        for( ComProperty* prop : pg.propList )
        {
            QString val = prop->toString();
            if( val.isEmpty() ) continue;
            item += "; "+prop->idStr()+"="+val;
        }
    }*/

    for( ComProperty* prop : m_propVector )
    {
        QString val = prop->toString();
        /// if( val.isEmpty() ) continue;
        item += "; "+prop->idStr()+"="+val;
    }
    item += "\n";

    return item;
}
