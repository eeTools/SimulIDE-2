/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <math.h>

#include "f_unaryop.h"

fUnaryOp::fUnaryOp()
        : fModule()
{
    setSize( 2 );
}
fUnaryOp::~fUnaryOp(){}

void fUnaryOp::connectSignal( fModule* signalModule, int slotId )
{
    m_input = signalModule->getOutputINT();  // Our input is the output of another module
}

void fUnaryOp::initModule()
{
    m_input = 0;
    m_outputINT = 0;
}

void fUnaryOp::updateOutput()
{
    int out = 0;
    int bits = 0;
    if( m_unOpType < NOT ) for( int i=0; i<m_bits; ++i ) if( *m_input & 1<<i ) bits++; // Logic ops

    switch( m_unOpType ) {
        case AND: out = bits == m_bits; break;
        case  OR: out = bits  > 0;      break;
        case XOR: out = bits == 1;      break;
        case NOT: out = !*m_input;       break;
        case INC: out = *m_input+1;      break;
        case DEC: out = *m_input-1;      break;
        case RTL: out = *m_input<<1;     break;
        case RTR: out = *m_input>>1;     break;
        case DTB: out = dtb();          break;
        case BTD: out = 1<<*m_input;     break;
    }

    if( m_outputINT == out ) return;
    m_outputINT = out;
    fModule::outputChanged();
}

int fUnaryOp::dtb()
{
    int i = m_bits;
    for( ; i>=0; --i ) if( *m_input & 1<<i ) break ;
    return i;
}

void fUnaryOp::setSize( int bits )
{
    if     ( bits < 1  ) bits = 1;
    else if( bits > 32 ) bits = 32;

    m_bits = bits;
    m_mask = pow( 2, m_bits )-1;
}

