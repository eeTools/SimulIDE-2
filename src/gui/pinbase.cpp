/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QFontMetrics>
#include <QCursor>
#include <QBrush>

#include "pinbase.h"
#include "lachannel.h"
#include "wirebase.h"
#include "simulator.h"

PinBase::PinBase( int angle, QPoint pos, QString id, QGraphicsItem* parent, int length )
       : QGraphicsItem( parent )
       , m_label( parent )
{
    m_blocked = false;
    m_unused  = false;
    m_animate = false;
    m_warning = false;

    m_pinType = pinNormal;
    m_wire  = nullptr;
    m_angle  = angle;
    m_space = 0;
    m_Hflip = 1;
    m_Vflip = 1;
    m_overScore = -1;
    m_wireFlags = 0;

    QFont font;
    font.setFamily("Ubuntu Mono");
    font.setPixelSize(7);
    font.setStretch( 100 );
    font.setLetterSpacing( QFont::PercentageSpacing, 100 );
#ifndef Q_OS_UNIX
    font.setLetterSpacing( QFont::PercentageSpacing, 87 );
    //font.setStretch( 99 );
#endif
    m_label.setFont( font );
    m_label.setText("");
    m_label.setBrush( QColor( 0, 0, 0 ) );//( QColor( 250, 250, 200 ) );

    setWire( nullptr );
    setPos( pos );
    setRotation( 180-angle );
    PinBase::setLength( length );
    setCursor( Qt::CrossCursor );
    setFlag( QGraphicsItem::ItemIsSelectable, false );
}
PinBase::~PinBase(){}

void PinBase::writeWireFlag( int flag, bool val )
{
    if( val ) m_wireFlags |= flag;
    else      m_wireFlags &= !(~flag);
}

void PinBase::setUnused( bool unused )
{
    m_unused = unused;
    if( unused ) setCursor( Qt::ArrowCursor );
    else         setCursor( Qt::CrossCursor );
    update();
}

void  PinBase::setWire( WireBase* wire )
{
    m_wire = wire;
    m_warning = false;

    if( m_wire ) setCursor( Qt::ArrowCursor );
    else         setCursor( Qt::CrossCursor );
}

void PinBase::flip( int h, int v )
{
    m_Hflip = h;
    m_Vflip = v;
    setLabelPos();
}

void PinBase::isMoved()
{
    if( m_wire ) m_wire->updateConRoute( this );
    setLabelPos();
}

void PinBase::setLabelText( QString label, bool over )
{
    label = label.simplified();
    m_labelText = label;
    m_overScore = -1;
    if( over && label.startsWith("!")) // Draw overscore
    {
        //if( !m_inverted ) m_overScore = 1; // label.indexOf("!");
        label.remove( 0, 1 );// replace("!","");
    }
    /// Hack: if ItemStacksBehindParent then overscore does not paint
    if( m_pinType != pinHook ) setFlag( QGraphicsItem::ItemStacksBehindParent, m_overScore == -1 );
    m_label.setText( label );
    setLabelPos();
}

void PinBase::setSpace( double s )
{
    m_space = s;
    setLabelPos();
}

void PinBase::setLabelPos()
{
    QFontMetrics fm( m_label.font() );

    double xlabelpos = pos().x();
    double ylabelpos = pos().y();
    m_labelheight = (fm.height()+1)/2;
    m_labelWidth  = fm.width(m_label.text());
    double space  = m_space ? m_space : (( m_labelheight < 4 ) ? 2 : m_labelheight/2);
    double offset = m_length + space;

    if( m_angle == 0 )         // Right side
    {
        m_label.setRotation( 0 );
        if( m_Hflip == -1 ) xlabelpos -= offset;
        else                xlabelpos -= m_labelWidth+offset;
        if( m_Vflip == -1 ) ylabelpos += m_labelheight;
        else                ylabelpos -= m_labelheight;
    }
    else if( m_angle == 90 )   // Top
    {
        m_label.setRotation(m_angle);
        if( m_Hflip == -1 ) xlabelpos -= m_labelheight;
        else                xlabelpos += m_labelheight;
        if( m_Vflip == -1 ) ylabelpos += m_labelWidth+offset;
        else                ylabelpos += offset;
    }
    else if( m_angle == 180 )  // Left
    {
        m_label.setRotation( 0 );
        if( m_Hflip == -1 ) xlabelpos += m_labelWidth+offset;
        else                xlabelpos += offset;
        if( m_Vflip == -1 ) ylabelpos += m_labelheight;
        else                ylabelpos -= m_labelheight;
    }
    else if( m_angle == 270 )  // Bottom
    {
        m_label.setRotation( m_angle );
        if( m_Hflip == -1 ) xlabelpos += m_labelheight;
        else                xlabelpos -= m_labelheight;
        if( m_Vflip == -1 ) ylabelpos -= m_labelWidth+offset;
        else                ylabelpos -= offset;
    }
    m_label.setPos( xlabelpos, ylabelpos );
    m_label.setTransform( QTransform::fromScale( m_Hflip, m_Vflip ) );
    update();
}

void PinBase::setLabelColor( QColor color ) { m_label.setBrush( QBrush(color) ); }

void PinBase::setFontSize( int size )
{
    QFont font = m_label.font();
    font.setPixelSize( size );
    m_label.setFont( font );
}

void PinBase::setPinAngle( int angle )
{
    m_angle= angle;
    setRotation( 180-angle );
}

void PinBase::setY( qreal y )
{
    QGraphicsItem::setY( y );
    isMoved();
}

void PinBase::moveBy( int dx, int dy )
{
    m_label.moveBy( dx, dy );
    QGraphicsItem::moveBy( dx, dy );
}

void PinBase::setLength( int length )
{
    if( length < 1 ) length = 1;
    m_length = length;
    int aLength = 11;
    if( length == 1 ) aLength = 6;
    m_area = QRect(-3, -3, aLength, 6);
    setLabelPos();
}

void PinBase::setVisible( bool visible )
{
    m_label.setVisible( visible );
    QGraphicsItem::setVisible( visible );
}

void PinBase::warning( bool w )
{
    m_warning = w;
    if     (  m_warning ) Simulator::self()->addToUpdateList( this );
    else if( !m_animate ) Simulator::self()->remFromUpdateList( this );
    update();
}

void PinBase::animate( bool an )
{
    if( m_unused ) return;
    m_animate = an;

    if     (  m_animate ) Simulator::self()->addToUpdateList( this );
    else if( !m_warning ) Simulator::self()->remFromUpdateList( this );

    update();
}
