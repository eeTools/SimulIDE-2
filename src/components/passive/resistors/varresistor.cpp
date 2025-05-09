/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "varresistor.h"
#include "simulator.h"

#include "doubleprop.h"
#include "propdialog.h"

#define tr(str) simulideTr("VarResistor",str)

listItem_t VarResistor::libraryItem(){
    return {
        tr("Variable Resistor"),
        "Resistors",
        "varresistor.png",
        "VarResistor",
        [](QString id){ return (Component*)new VarResistor("VarResistor", id ); } };
}

VarResistor::VarResistor( QString type, QString id )
           : VarResBase( type, id  )
{
    setVal( 0 );

    addPropGroup( { tr("Main"), {
        new DoubProp<VarResistor>("Min_Resistance", tr("Minimum Resistance"), "Ω"
                                 , this, &VarResistor::minVal, &VarResistor::setMinVal ),

        new DoubProp<VarResistor>("Max_Resistance", tr("Maximum Resistance"), "kΩ"
                                 , this, &VarResistor::maxVal, &VarResistor::setMaxVal ),

        new DoubProp<VarResistor>("Value_Ohm", tr("Current Value"), "Ω"
                                 , this, &VarResistor::getVal, &VarResistor::setVal ),

        new DoubProp<VarResistor>("Dial_Step", tr("Dial Step"), "Ω"
                                 , this, &VarResistor::getStep, &VarResistor::setStep ),
    },0 } );

    addPropGroup( { tr("Dial"), Dialed::dialProps(), groupNoCopy } );
}
VarResistor::~VarResistor(){}

void VarResistor::updateStep()
{
    if( !m_needUpdate ) return;
    m_needUpdate = false;

    eResistor::setResistance( m_value );
    if( m_propDialog ) m_propDialog->updtValues();
    else setValLabelText( getPropStr( showProp() ) );
}

void VarResistor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    if( m_hidden ) return;
    Component::paint( p, o, w );

    p->drawRect( QRectF(-11,-4.5, 22, 9 ) );
    p->drawLine(-6, 6, 8,-8 );
    p->drawLine( 8,-6, 8,-8 );
    p->drawLine( 8,-8, 6,-8 );

    Component::paintSelected( p );
}
