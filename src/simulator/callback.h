/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( All Rights Reserved )*************************************************/

#ifndef CALLBACK_H
#define CALLBACK_H

#include <inttypes.h>

class CallBack
{
    //friend class SimEngine;
    friend class Simulator;

    public:
        CallBack() { next = nullptr; }
        virtual ~CallBack(){;}

        virtual void call(){;}
        virtual void call(int){;}

        CallBack* next; // Used by SimEngine

    //protected:
        uint64_t time;
};

template <class Obj>
class SimEvent : public CallBack
{
    public:
        SimEvent( Obj* e, void(Obj::*cb)() )
        {
            element  = e;
            callback = cb;
        }
        virtual void call() override
        { (element->*callback)(); }

    private:
        Obj* element;
        void(Obj::*callback)();
};

template <class Obj>
class SimEventInt : public CallBack
{
    public:
        SimEventInt( Obj* e, void(Obj::*cb)(SimEventInt*) )
        {
            element  = e;
            callback = cb;
            nextInt  = nullptr;
        }
        virtual void call() override
        { (element->*callback)(this); }

        int valInt;
        SimEventInt<Obj>* nextInt; // Used by Elements

    private:
        Obj* element;
        void(Obj::*callback)(SimEventInt*);
};

template <class Obj>
class changeInt : public CallBack
{
    public:
        changeInt( Obj* e, void(Obj::*cb)(int) )
        {
            element  = e;
            callback = cb;
            next  = nullptr;
        }
        virtual void call( int v ) override
        { (element->*callback)(v); }

    private:
        Obj* element;
        void(Obj::*callback)(int);
};

/*template <class E>
class voltChangedCB : public CallBack
{
    public:
    E* element;
    void(E::*callbackV)( int, double);

    virtual void callV( int i, double v ) override
    { (element->*callbackV)( i, v ); }
};*/

#endif
