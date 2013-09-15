#include "ViewLine.h"

#include <QPainterPathStroker>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

namespace Morf
{
  
  ViewLine::ViewLine(const QLineF& line)
  {
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    
    resetMoveState(); // reset state
    
    updateLine_prim(line);
  }
  
  ViewLine::~ViewLine()
  {
  }
  
  QRectF ViewLine::boundingRect() const
  {
    return m_BoundingPath.boundingRect();
  }
  
  QPainterPath ViewLine::shape() const
  {
    return m_BoundingPath;
  }
  
  void ViewLine::paint(QPainter* painter, 
                       const QStyleOptionGraphicsItem* option, 
                       QWidget* widget)
  {
    option = 0;
    widget = 0;
    
    QBrush brush0(Qt::blue);
    QBrush brush1(Qt::red);
    QPen line_pen(brush0, 3);
    
    painter->setPen(line_pen);
    painter->drawLine(m_Line);
    
    painter->setPen(QPen(Qt::transparent));
    painter->setBrush(brush0);
    painter->drawEllipse(m_EndPt0);
    
    painter->setBrush(brush1);
    painter->drawEllipse(m_EndPt1);
  }
  
  void ViewLine::updateLine(const QLineF& line)
  {
    // notify the scene that the geometry is to be changed
    this->prepareGeometryChange();    
    updateLine_prim(line);
    emit changed(line);
  }
  
  void ViewLine::updateLine_prim(const QLineF& line)
  {
    const qreal rect_halfsize = 5.0;
    const qreal rect_size = rect_halfsize * 2;
    const QPointF vec(rect_halfsize, rect_halfsize);
    
    QPointF pt0 = line.p1();
    QPointF pt1 = line.p2();
    
    this->setPos(pt0);
    pt0 = this->mapFromScene(pt0);
    pt1 = this->mapFromScene(pt1);
    
    m_Line = QLineF(pt0, pt1);
    m_EndPt0 = QRectF(pt0-vec, QSizeF(rect_size, rect_size));
    m_EndPt1 = QRectF(pt1-vec, QSizeF(rect_size, rect_size));
    
    QPainterPath path(pt0);
    path.lineTo(pt1);
    
    QPainterPathStroker stroker;
    stroker.setWidth(3.0);
    m_BoundingPath = stroker.createStroke(path);
    m_BoundingPath.addEllipse(m_EndPt0);
    m_BoundingPath.addEllipse(m_EndPt1);
  }
  
  // BEGIN -- Move Event
  
  void ViewLine::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
  {
    dispatchMoveEvent(event);
    event->accept();
  }
  
  void ViewLine::mousePressEvent(QGraphicsSceneMouseEvent* event)
  {
    if ( m_EndPt1.contains( event->pos() ) )
    {
      m_MoveState = MY_EVT_MOVE_PT1;
      event->accept();
    }
    else if ( m_EndPt0.contains ( event->pos() ) )
    {
      m_MoveState = MY_EVT_MOVE_PT0;
      event->accept();
    }
    else
    {
      m_MoveState = MY_EVT_MOVE_LINE;
      event->accept();
    }
  }
  
  void ViewLine::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
  {
    dispatchMoveEvent(event);
    event->accept();
    resetMoveState();
  }
  
  void ViewLine::resetMoveState()
  {
    m_MoveState = MY_EVT_NONE;
  }
  
  void ViewLine::dispatchMoveEvent(QGraphicsSceneMouseEvent* event)
  {
    QPointF delta = event->scenePos() - event->lastScenePos();
    QLineF line = mapLineToScene(m_Line);
    
    switch(m_MoveState)
    {
    case MY_EVT_MOVE_LINE:
      line.translate(delta);
      updateLine(line);
      break;
    case MY_EVT_MOVE_PT0:
      updateLine(QLineF(line.p1() + delta, line.p2()));
      break;
    case MY_EVT_MOVE_PT1:
      updateLine(QLineF(line.p1(), line.p2() + delta));
      break;
    default:
      break;
    }
  }
  
  // END   -- Move Event
  
}
