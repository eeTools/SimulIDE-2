/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

union value_t{
    int64_t intVal;
    double  dblVal;
    bool    boolVal;
    QString* strVal;
};

enum pType_t{
    P_Uint = 0,
    P_Int,
    P_Double,
    P_Bool,
    P_Icon,
    P_Enum,
    P_Point,
    P_String
};

struct param_t{
    QString idStr;
    QString label;
    QString unit;
    void*   data;
    uint8_t type;
    uint8_t flags;
};

enum flags_t{
    P_Hidden = 1,
    P_NoCopy = 1<<1,
    P_Signal = 1<<2,
    P_Slot   = 1<<3,
    P_Pkg    = 1<<4,
    P_NoSet  = 1<<5
};
