/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#include <math.h>

#include "f_binaryop.h"


fBinaryOp::fBinaryOp()
         : fModule()
{
    m_inputA = nullptr;
    m_inputB = nullptr;

    m_binOpType = AND;

    setSize( 2 );
}
fBinaryOp::~fBinaryOp(){}

void fBinaryOp::connectSignal( fModule* signalModule, int slotId )
{
    if     ( slotId == 0 ) m_inputA = signalModule->getOutputINT();  // Our input is the output of another module
    else if( slotId == 1 ) m_inputB = signalModule->getOutputINT();  // Get pointer to output in signal module
}

void fBinaryOp::initModule()
{
    m_outputINT = 0;
}

void fBinaryOp::updateOutput()
{
    int out = 0;

    switch( m_binOpType )
    {
        case AND: out = *m_inputA & *m_inputB; break;
        case  OR: out = *m_inputA | *m_inputB; break;
        case XOR: out = *m_inputA ^ *m_inputB; break;
        case ADD: out = *m_inputA + *m_inputB; break;
        case SUB: out = *m_inputA - *m_inputB; break;
        case MUL: out = *m_inputA * *m_inputB; break;
        case DIV: out = *m_inputA / *m_inputB; break;
        case MOD: out = *m_inputA % *m_inputB; break;
        case CMP: out = cmp();                 break;
    }

    if( m_outputINT == out ) return;
    m_outputINT = out;

    fModule::outputChanged();
}

int fBinaryOp::cmp()
{
    int diff = *m_inputA - *m_inputB;
    if( diff  < 0 ) return 1;
    if( diff == 0 ) return 2;
    /* diff  > 0 */ return 4;

}

void fBinaryOp::setSize( int bits )
{
    if     ( bits < 1  ) bits = 1;
    else if( bits > 32 ) bits = 32;

    m_bits = bits;
    m_mask = pow( 2, m_bits )-1;
}
