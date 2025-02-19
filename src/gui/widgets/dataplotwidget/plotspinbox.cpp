/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "plotspinbox.h"
//#include <QDebug>

PlotSpinBox::PlotSpinBox( QWidget* parent )
           : QDoubleSpinBox( parent )
{
    m_this = this;
}
PlotSpinBox::~PlotSpinBox(){;}

QString PlotSpinBox::textFromValue( double value ) const
{
    return QString::number( value ); // This removes extra zeros
}

QValidator::State PlotSpinBox::validate( QString& text, int& n ) const
{
    QStringList l = text.split(" ");
    QString val = l.first();
    QString suf = l.last();

    if( !suf.isEmpty() && n == val.length() )
    {
        QString mult = val.right(1);
        if     ( mult == "p") { val.replace( n-1, 1,""); m_this->setSuffix(" p"+m_unit); }
        else if( mult == "n") { val.replace( n-1, 1,""); m_this->setSuffix(" n"+m_unit); }
        else if( mult == "u") { val.replace( n-1, 1,""); m_this->setSuffix(" µ"+m_unit); }
        else if( mult == "m") { val.replace( n-1, 1,""); m_this->setSuffix(" m"+m_unit); }
        else if( mult == "k") { val.replace( n-1, 1,""); m_this->setSuffix(" k"+m_unit); }
        else if( mult == "M") { val.replace( n-1, 1,""); m_this->setSuffix(" M"+m_unit); }
        else if( mult == "G") { val.replace( n-1, 1,""); m_this->setSuffix(" G"+m_unit); }
        else if( mult == m_unit ) { val.remove(m_unit); m_this->setSuffix(" "+m_unit); }
        else{
            QString unitL = m_unit.toLower();
            if( val.contains( unitL ) ) { val.remove(unitL); m_this->setSuffix(" "+m_unit); }
        }
    }
    text = val;
    if( !suf.isEmpty() ) text += " "+suf;

    QValidator::State state = QDoubleSpinBox::validate( val, n );
    return state;
}

