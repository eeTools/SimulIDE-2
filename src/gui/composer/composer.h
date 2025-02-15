/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef COMPOSER_H
#define COMPOSER_H

#include "canvasbase.h"
#include <QSet>

class ComposerView;
class fComponent;
class FuncBlock;
class Wire;
class Module;
class Hook;

class Composer : public CanvasBase
{
    public:
        Composer( qreal w, qreal h, ComposerView* parent );
        ~Composer();

        void clearCanvas() override;

        FuncBlock* createBlock( QString type, int id=-1 );
        void       removeBlock( FuncBlock* fb );

        WireBase* newWire( int id, PinBase* startPin, PinBase* endPin ) override;

        void loadComponent( QString path );

        QString toString() override;

 static Composer* self() { return m_pSelf; }

    protected:
        void dropEvent( QGraphicsSceneDragDropEvent* event ) override;

    private:
 static Composer* m_pSelf;

        void loadStrDoc( QString doc ) override;

        //Hook* getHook( QString id );

        QSet<FuncBlock*> m_blockList;
};
#endif
