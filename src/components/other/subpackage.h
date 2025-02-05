/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef SUBPACKAGE_H
#define SUBPACKAGE_H

#include "chip.h"
#include "linker.h"

class LibraryItem;
class QAction;

class SubPackage : public Chip, public Linker
{
        friend class Circuit;

    public:
        SubPackage( QString id );
        ~SubPackage();
        
 static listItem_t registerItem();

        int width() { return m_width; }
        void setWidth( int width );

        int height() { return m_height; }
        void setHeight( int height );

        QString packageFile() { return m_pkgeFile; }
        void setPackageFile( QString package );

        QString bckGndData() { return m_BckGndData; }
        void setBckGndData( QString data ) override;
        void setBackground( QString bck ) override;

        void setLogicSymbol( bool ls ) override;

        QString packagePins();
        void setPackagePins( QString pinsStr );

        QList<Pin*> getPins() override { QList<Pin*> p; return p; } // Used to access wires (we have no wires)

        void setEventPin( Pin* pin ) { m_eventPin = pin; }

        void savePackage( QString fileName );

        void setSubcTypeStr( QString s ) override;

        void compSelected( Component* comp ) override;  // Use link mechanism to select main components

        /// virtual void moveSignal() override {;}

       void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

        void invertPin( bool invert );
        void setPinId( QString id );
        void setPinName( QString name );
        void setPinAngle( int i );
        void setPinSpace( double space );
        void boardModeSlot();
        void setBoardMode( bool mode );
        void mainComp() { Linker::startLinking(); }
        void unusePin( bool unuse );
        void setPointPin( bool point );
        void setBusPin( bool bus );
        void editPin();
        void deleteEventPin();
    
    protected:
        void loadPackage();
        void slotSave();
        void editFinished( int r );

        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;
        void hoverMoveEvent( QGraphicsSceneHoverEvent* event ) override;
        void hoverLeaveEvent( QGraphicsSceneHoverEvent* event ) override;
        
        void contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu ) override;

        void addNewPin( QString id, QString type, QString label,
                            int pos, int xpos, int ypos, int angle, int length=8, int space=0 ) override;

    private:
        QString pinEntry( Pin* pin );
        QString pinStrEntry( Pin* pin );
        QString adjustSize( QString str, int size );

 static QString m_lastPkg;

        bool m_fakePin; // Data for drawing pin when hovering

        int m_angle;  
        int m_p1X;
        int m_p1Y;
        int m_p2X;
        int m_p2Y;

        bool m_circPosSaved;
        bool m_boardMode;
        QAction* m_boardModeAction;

        QString m_pkgeFile;     // file containig package defs

        QString m_BckGndData;   // Embedded background png data

        Pin* m_eventPin;
        QList<Pin*> m_pkgePins;
};

#include <QDialog>

class QLabel;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;

class EditDialog : public QDialog
{
    public:
        EditDialog( SubPackage* pack, Pin* eventPin, QWidget* parent = 0 );

    private:
        void setPointPin( bool p );
        void setBusPin( bool b );

        SubPackage* m_package;

        QLabel*    m_nameLabel;
        QLineEdit* m_nameLineEdit;

        QLabel*    m_idLabel;
        QLineEdit* m_idLineEdit;

        QLabel*    m_spaceLabel;
        QDoubleSpinBox* m_spaceBox;

        QLabel*    m_angleLabel;
        QComboBox* m_angleBox;

        QCheckBox* m_invertCheckBox;
        QCheckBox* m_unuseCheckBox;
        QCheckBox* m_pointCheckBox;
        QCheckBox* m_busCheckBox;
};

#endif
