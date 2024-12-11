/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "board.h"
#include "circuit.h"

BoardSubc::BoardSubc( QString id )
         : SubCircuit( id )
{
    m_graphical = true;
    m_subcType = Chip::Board;
}
BoardSubc::~BoardSubc(){}

void BoardSubc::setBoard( BoardSubc* board )
{
    m_parentBoard = board;
    setParentItem( board );
    Circuit::self()->compList()->removeOne( this );
}
