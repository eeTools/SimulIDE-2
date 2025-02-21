/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "plotbase.h"
#include "plotdisplay.h"
#include "simulator.h"
#include "circuit.h"
#include "circuitwidget.h"
#include "propdialog.h"
#include "iopin.h"
#include "utils.h"

#define tr(str) simulideTr("PlotBase",str)

PlotBase::PlotBase( int id )
        : Component( id )
        //, ScriptBase( id )
{
    m_graphical = true;
    m_bufferSize = 600000;

    m_connectGnd = true;
    m_inputImped = 1e-7;

    //m_doTest = false;
    m_testTime = 0;

    m_color[0] = QColor( 240, 240, 100 );
    m_color[1] = QColor( 220, 220, 255 );
    m_color[2] = QColor( 255, 210, 90  );
    m_color[3] = QColor( 000, 245, 160 );
    m_color[4] = QColor( 255, 255, 255 );

    m_baSizeX = 135;
    m_baSizeY = 135;

    //int r;
    /*m_pauseFunc = nullptr;
    m_aEngine->RegisterObjectType("PlotBase",0, asOBJ_REF | asOBJ_NOCOUNT );
    m_aEngine->RegisterGlobalProperty("PlotBase pb", this );
    m_aEngine->RegisterObjectProperty("PlotBase", "bool m_pause", asOFFSET(PlotBase,m_pause)); //assert( r >= 0 );
    m_aEngine->RegisterObjectProperty("PlotBase", "int ch1", asOFFSET(PlotBase,m_condCh1)); //assert( r >= 0 );
    m_aEngine->RegisterObjectProperty("PlotBase", "int ch2", asOFFSET(PlotBase,m_condCh2)); //assert( r >= 0 );
    m_aEngine->RegisterObjectProperty("PlotBase", "int ch3", asOFFSET(PlotBase,m_condCh3)); //assert( r >= 0 );
    m_aEngine->RegisterObjectProperty("PlotBase", "int ch4", asOFFSET(PlotBase,m_condCh4)); //assert( r >= 0 );
    m_aEngine->RegisterObjectProperty("PlotBase", "int ch5", asOFFSET(PlotBase,m_condCh5)); //assert( r >= 0 );
    m_aEngine->RegisterObjectProperty("PlotBase", "int ch6", asOFFSET(PlotBase,m_condCh6)); //assert( r >= 0 );
    m_aEngine->RegisterObjectProperty("PlotBase", "int ch7", asOFFSET(PlotBase,m_condCh7)); //assert( r >= 0 );
    m_aEngine->RegisterObjectProperty("PlotBase", "int ch8", asOFFSET(PlotBase,m_condCh8)); //assert( r >= 0 );
*/
    QString n;
    for( int i=1; i<9; ++i ) // Condition simple to script: ch1l to (ch1==1)
    {
        n = QString::number(i);
        m_condTo["ch"+n+"l"] = "(pb.ch"+n+"==1)";
        m_condTo["ch"+n+"r"] = "(pb.ch"+n+"==2)";
        m_condTo["ch"+n+"h"] = "(pb.ch"+n+"==3)";
        m_condTo["ch"+n+"f"] = "(pb.ch"+n+"==4)";
    }
    Simulator::self()->addToUpdateList( this );

    m_timeStep = 1000;
    m_autoExport = false;
    m_exportFile = changeExt( Circuit::self()->getFilePath(), "_"+QString::number( id )+".vcd" );

    /*addPropGroup( { tr("Main"), {
        new IntProp ("Basic_X"   ,tr("Screen Width px")  , "", this,0,"uint" ),
        new IntProp ("Basic_Y"   ,tr("Screen Height px") , "", this,0,"uint" ),
        new IntProp ("BufferSize",tr("Buffer Size")      , "", this,0,"uint" ),
        new BoolProp("connectGnd",tr("Connect to ground"), "", this,0 ),
        new DoubProp("InputImped",tr("Impedance")        ,"Ω", this )
    }, groupNoCopy} );

    addPropGroup( { tr("Test"), {
        new IntProp("TestTime",tr("Test Time"),"", this,0,"uint" ),
    }, 0 } );

    addPropGroup( {"Hidden", {
        new StrProp("TimDiv","","", this ),
        new StrProp("TimPos","","", this ),
        new StrProp("VolDiv","","", this ),
        new StrProp("Conds","","", this ),
        new StrProp("Tunnels","","", this ),
        new IntProp("Trigger","","", this ),
       new StrProp("TestData","","", this ),
    }, groupHidden } );*/
}
PlotBase::~PlotBase()
{
    for( int i=0; i<m_numChannels; i++ ) delete m_channel[i];
}

bool PlotBase::setPropStr( QString prop, QString val )
{
    if     ( prop =="hTick"  ) setTimeDiv( val.toLongLong()*1e3 ); // Old: TODELETE
    else if( prop =="vTick"  ) setVolDiv( val );
    else if( prop =="TimePos") setTimPos( val+"000" );
    else return Component::setPropStr( prop, val );
    return true;
}

void PlotBase::initialize()
{
    /// if( m_testTime ) Simulator::self()->addEvent( m_testTime*1000, this );
}

/*void PlotBase::runEvent() // Test time reached, make comparison
{
    for( int i=0; i<m_numChannels; ++i )
    {
        if( !m_channel[i]->doTest() )
        {
            qDebug() << "PlotBase::runEvent Error: Test failed for Channel" << i;
        }
    }
}*/

QString PlotBase::testData()
{
    QString td;
    if( !m_testTime ) return td;

    for( int i=0; i<m_numChannels; ++i ) td += m_channel[i]->testData()+";";
    td.remove( td.size()-1, 1);
    return td;
}

void PlotBase::setTestData( QString td )
{
    if( td.isEmpty() ) return;

    QStringList chDataList = td.split(";");
    for( int i=0; i<chDataList.size(); ++i )
    {
        if( i == m_numChannels ) break;
        m_channel[i]->setTestData( chDataList.at(i) );
    }
}

void PlotBase::setBaSizeX( int size )
{
    if( size < 135 ) size = 135;
    m_baSizeX = size;
    expand( m_expand );
}

void PlotBase::setBaSizeY( int size )
{
    if( size < 135 ) size = 135;
    m_baSizeY = size;
    expand( m_expand );
}

void PlotBase::setBufferSize( int bs )
{
    if( bs < 0 || bs > 10000000 ) bs = 10000000;
    else if( bs < 1000 ) bs = 1000;
    m_bufferSize = bs;
    for( int i=0; i<m_numChannels; i++ )
    {
        m_channel[i]->m_buffer.resize( m_bufferSize );
        m_channel[i]->m_time.resize( m_bufferSize );
    }
}

void PlotBase::setConnectGnd( bool c )
{
    if( m_connectGnd == c ) return;
    m_connectGnd = c;
    updtProperties();
    m_changed = true;
}

void PlotBase::setInputImped( double i )
{
    if( m_inputImped == i ) return;
    if( i  < 0 ) return;
    m_inputImped = i;
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

QString PlotBase::timDiv()
{
    return QString::number( m_timeDiv );
}

void PlotBase::setTimDiv( QString td )
{
    setTimeDiv( td.toLongLong() );
}

void PlotBase::setTimeDiv( uint64_t td )
{
    m_display->setTimeDiv( td );
    m_timeDiv = m_display->m_timeDiv;
}

QString PlotBase::tunnels()
{
    QString list;
    for( int i=0; i<m_numChannels; ++i ) list.append( m_channel[i]->m_chTunnel ).append(",");
    return list;
}

void PlotBase::updateConds( QString conds )
{
    ///m_pauseFunc = nullptr;
    m_pause = false;
    m_conditions = conds;

    if( conds.isEmpty() ) return;
    conds = conds.toLower();

    QString n;
    for( int i=1; i<9; ++i ) // Condition simple to script: ch1l to (ch1==1)
    {
        n = QString::number(i);
        conds.replace( "ch"+n+"l", m_condTo.value("ch"+n+"l") );
        conds.replace( "ch"+n+"r", m_condTo.value("ch"+n+"r") );
        conds.replace( "ch"+n+"h", m_condTo.value("ch"+n+"h") );
        conds.replace( "ch"+n+"f", m_condTo.value("ch"+n+"f") );
    }
    /*m_script = "void pause() { pb.m_pause = "+conds+";}";
    /// qDebug() << m_script <<endl;
    int r = ScriptBase::compileScript();
    if( r < 0 ) { qDebug() << "PlotBase::updateConds Failed to compile expression:"<<conds; return; }

    m_pauseFunc = m_aEngine->GetModule(0)->GetFunctionByDecl("void pause()");*/
}

void PlotBase::conditonMet( int ch, cond_t cond )
{
    /// if( !m_pauseFunc )
        return;

    ch++;
    switch ( ch ) {
        case 1: m_condCh1 = (int)cond; break;
        case 2: m_condCh2 = (int)cond; break;
        case 3: m_condCh3 = (int)cond; break;
        case 4: m_condCh4 = (int)cond; break;
        case 5: m_condCh5 = (int)cond; break;
        case 6: m_condCh6 = (int)cond; break;
        case 7: m_condCh7 = (int)cond; break;
        case 8: m_condCh8 = (int)cond; break;
        default: break;
    }
    if( Simulator::self()->simState() <= SIM_PAUSED ) return;

    /// callFunction( m_pauseFunc ); // Check if condition met:

    if( m_pause )
    {
        m_risEdge = Simulator::self()->circTime();
        CircuitWidget::self()->pauseCirc();
        if( m_autoExport ) QTimer::singleShot( 50, [=](){ dump(); } );
    }

    //if( m_trigger != 8 ) return;

    /*if( m_conditions == m_condTarget ) // All conditions met
    {                                  // Trigger Pause Simulation
        m_risEdge = Simulator::self()->circTime();
        CircuitWidget::self()->pauseSim();
    }*/
    /*else  // Rising will be High and Falling Low in next cycles
    {
        if     ( cond == C_RISING )  m_conditions[ch] = C_HIGH;
        else if( cond == C_FALLING ) m_conditions[ch] = C_LOW;
    }*/
}

void PlotBase::slotProperties()
{
    Component::slotProperties();
    updtProperties();
}

void PlotBase::updtProperties()
{
    if( !m_propDialog ) return;
    m_propDialog->showProp("InputImped", m_connectGnd );
    m_propDialog->adjustWidgets();
}

void PlotBase::remove()
{
    if( m_expand ) expand( false );
    Component::remove();
}

void PlotBase::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );
    
    //p->setBrush( Qt::darkGray );
    p->setBrush(QColor( 230, 230, 230 ));
    p->drawRoundedRect( m_area, 4, 4 );
    
    p->setBrush( Qt::white );
    QPen pen = p->pen();
    pen.setWidth( 0 );
    pen.setColor( Qt::white );
    p->setPen(pen);

    Component::paintSelected( p );
}
