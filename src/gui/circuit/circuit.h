/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <QTimer>

#include "canvasbase.h"
#include "component.h"
#include "wire.h"
#include "pin.h"


class creCompDialog;
class CircuitView;
class SubPackage;
class SubCircuit;
class Simulator;
class Node;

class Circuit : public CanvasBase
{
    friend class SubCircuit;
    friend class Simulator;

    Q_OBJECT

    public:
        Circuit( int width, int height, CircuitView* parent );
        ~Circuit();

        bool drawGrid() { return !m_hideGrid; }
        void setDrawGrid( bool draw );

        bool animate() { return m_animate; }
        void setAnimate( bool an );

        int sceneWidth() { return m_sceneWidth; }
        void setSceneWidth( int w );

        int sceneHeight() { return m_sceneHeight; }
        void setSceneHeight( int h );

        int autoBck();
        void setAutoBck( int secs );

        void removeItems() override;
        void removeComp( Component* comp) ;
        void compRemoved( bool removed ) { m_compRemoved = removed; }
        void clearCanvas() override;

        //--- Undo/Redo ----------------------------------
        void calculateChanges() override; // Calculate total changes
        void beginUndoStep() override;      // Record current state
        //bool undoRedo() override { return m_undo || m_redo; }
        //------------------------------------------------

        void setChanged();

        void accepKeys( bool a ) { m_acceptKeys = a; }

        void loadCircuit( QString filePath );
        bool saveCircuit( QString filePath );

        Component* createItem( QString name, QString id, bool map=true );

        void addComponent( Component* comp );

        QList<Component*>* compList() { return &m_compList; }

        Component* getCompById( QString id );
        QString origId( QString name ) { return m_idMap.value( name ); } // used by Shield

        SubPackage* getBoard() { return m_board; }
        void setBoard( SubPackage* b ) { m_board = b; }

        SubCircuit* getSubcircuit() { return m_subCircuit; }
        void setSubcircuit( SubCircuit* sc ) { m_subCircuit = sc; }

        bool isSubc() { return m_createSubc; }

        QString getSeqNumber( QString name );

        const QString getFilePath() const { return m_filePath; }
        void setFilePath( QString f ) { m_filePath = f; }

        bool saveString( QString &fileName, QString doc );
        QString circuitToString();
        //QString circuitToComp( QString category, QString iconData, QString compType );

        Route* newWire( QString id, PinBase* startPin, PinBase* endPin ) override;

        bool isComp() { return m_creCompDialog != nullptr; }
        QString category() { return m_category; }
        QString compName() { return m_compName; }
        QString compInfo() { return m_compInfo; }

        QString iconData() { return m_iconData; }
        void setIconData( QString id ) { m_iconData = id; }

        int circuitRev() { return m_circRev; }

        void drawBackground( QPainter* painter, const QRectF &rect ) override;

 static Circuit* self() { return m_pSelf; }

    signals:
        void keyEvent( QString key, bool pressed );

    public slots:
        void importCircuit();
        void createComp();
        void cancelComp();
        //void bom();
        void saveBackup();

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) override;
        void mouseMoveEvent( QGraphicsSceneMouseEvent* event ) override;
        void keyPressEvent( QKeyEvent* event ) override;
        void keyReleaseEvent( QKeyEvent* event ) override;
        void dropEvent( QGraphicsSceneDragDropEvent* event ) override;

    private:
 static Circuit*  m_pSelf;

        void loadStrDoc( QString &doc );

        QString circuitHeader();
        //void updatePinName( QString* name );

        QString m_filePath;
        QString m_backupPath;

        QString m_itemType;
        QString m_category;
        QString m_compName;
        QString m_compInfo;
        QString m_iconData;

        CircuitView* m_graphicView;

        int m_circRev;
        int m_error;

        bool m_hideGrid;
        bool m_compRemoved;
        bool m_animate;
        bool m_acceptKeys;
        bool m_createSubc;

        QList<Component*> m_compList;   // Component list

        SubPackage* m_board;
        SubCircuit* m_subCircuit;

        QTimer m_bckpTimer;

        Simulator* m_simulator;

        creCompDialog* m_creCompDialog;

        //--- Undo/Redo ----------------------------------
        void restoreState() override;

        QList<Component*> m_oldComps;
};

#endif
