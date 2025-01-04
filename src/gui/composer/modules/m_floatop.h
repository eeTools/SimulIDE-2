/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef FLOATOP_H
#define FLOATOP_H

#include "module.h"
#include "modsignal.h"

class FloatOp : public Module
{
public:
    FloatOp( QString name );
    ~FloatOp();

    enum floatOpType_t{
        ADD,    // A + B
        SUB,    // A - B
        MUL,    // A * B
        DIV,    // A / B
        CMP     // A <=> B
    };

    static listItem_t registerItem();

    void initModule() override;
    void runStep() override;

    QString typeStr() { return m_floatOpStr; }
    void setTypeStr( QString type );

private:
    void updateOutput();

    inline double cmp();

    floatOpType_t m_floatOpType;
    QString       m_floatOpStr;
    QStringList   m_floatOpList;

    double m_inputA;
    double m_inputB;
    double m_output;

    ModSlot   m_inSlotA;
    ModSlot   m_inSlotB;
    ModSignal m_outSignal;
};
#endif
