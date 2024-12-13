/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef CANVASBASE_H
#define CANVASBASE_H

#include <QGraphicsScene>
#include <QSet>

#include "wirebase.h"

#define COMP_STATE_NEW "__COMP_STATE_NEW__"

class QGraphicsView;
class Node;
class WireBase;

class CanvasBase : public QGraphicsScene
{
    Q_OBJECT

    public:
        CanvasBase( qreal w, qreal h, QGraphicsView* parent );
        ~CanvasBase();

        struct prop_t{
            QString name;
            QString value;
        };

        QString newSceneId() { return QString::number(++m_seqNumber); }
        QString newWireId()  { return QString::number(++m_conNumber); }

        virtual void removeItems(){;}
        virtual void clearCanvas();

        void addNode( Node* node );
        void removeNode( Node* node );
        QList<Node*>* nodeList() { return &m_nodeList; }

        void startWire( PinBase* startpin, bool save=true );
        void closeWire( PinBase* endpin, bool save=false );
        void removeWire( WireBase* wire );
        void deleteNewWire();
        WireBase* getNewWire() { return m_newWire; }
        WireBase* createWire( QList<prop_t> properties, QString newUid );
        virtual WireBase* newWire( QString id, PinBase* startPin, PinBase* endPin )=0;

        QList<WireBase*>* wireList()  { return &m_wireList; }

        PinBase* findPin( int x, int y, QString id );
        void addPin( PinBase* pin, QString pinId ) { m_pinMap[ pinId ] = pin; m_LdPinMap[ pinId ] = pin; }
        void remPin( QString pinId ) { m_pinMap.remove( pinId ); }
        QString replaceId( QString pinName );

        bool pasting() { return m_pasting; }
        bool isBusy()  { return m_busy || m_pasting | m_deleting; }

        QHash<QString, CompBase*>* compMap() { return &m_compMap;}

        virtual QString toString(){ return "";}

        //--- Undo/Redo ----------------------------------
        void setChanged();
        void saveChanges();
        void removeLastUndo();
        void addCompChange(  QString component, QString property, QString undoVal );
        void saveCompChange( QString component, QString property, QString undoVal );
        void beginCircuitBatch();
        void endCircuitBatch();
        void cancelUndoStep();               // Revert changes done
        void endUndoStep();                  // Does create/remove
        virtual void beginUndoStep(){;}      // Record current state
        virtual void calculateChanges(){;}   // Calculate total changes
        bool undoRedo() { return m_undo || m_redo; }
        //------------------------------------------------

    public slots:
        void importCircuit();
        void copy( QPointF eventpoint );
        void paste( QPointF eventpoint );
        void undo();
        void redo();

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseMoveEvent( QGraphicsSceneMouseEvent* event ) override;

        virtual void loadStrDoc( QString doc ){;}
        bool saveStrDoc( QString fileName, QString doc );

        virtual void stop(){;}

        void setSize( int width, int height );

        int m_seqNumber;
        int m_conNumber;

        int m_sceneWidth;
        int m_sceneHeight;
        QRect m_scenerect;

        bool m_pasting;
        bool m_busy;
        bool m_deleting;
        bool m_loading;
        bool m_changed;

        QString m_filePath;

        QPointF m_eventpoint;
        QPointF m_deltaMove;

        WireBase* m_newWire;

        QMap<QString, PinBase*> m_pinMap;   // Pin Id to PinBase*
        QMap<QString, PinBase*> m_LdPinMap; // Pin Id to PinBase* while loading/pasting/importing
        QMap<QString, QString>  m_idMap;    // Component seqNumber to new seqNumber (pasting)

        QList<WireBase*> m_wireList;  // Wire list
        QList<Node*>  m_nodeList;  // Node list

        //--- Undo/Redo ----------------------------------
        struct compChange{      // Component Change to be performed by Undo/Redo to complete a Circuit change
            QString component;  // Component name
            QString property;   // Property name
            QString undoValue;  // Property value for Undo step
            QString redoValue;  // Property value for Redo step
        };
        struct circChange{       // Circuit Change to be performed by Undo/Redo to restore circuit state
            QList<compChange> compChanges;
            int size() { return compChanges.size(); }
            void clear() { compChanges.clear(); }
        };

        inline void clearCircChanges() { m_circChange.clear(); }
        void deleteRemoved();
        virtual void restoreState(){;}

        int m_maxUndoSteps;
        int m_undoIndex;
        int m_cicuitBatch;

        bool m_undo;
        bool m_redo;

        circChange m_circChange;
        QList<circChange> m_undoStack;

        QHash<QString, CompBase*> m_compMap;  // Component Id to Component*, used in UNDO/REDO

        QList<CompBase*>  m_removedComps; // removed component list;
        QList<WireBase*>  m_oldWires;
        QList<Node*>      m_oldNodes;
        QMap<CompBase*, QString> m_compStrMap;
};
#endif

