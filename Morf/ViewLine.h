#pragma once

#include <QObject>
#include <QGraphicsItem>
#include <QPainterPath>

namespace Morf
{
  
  class ViewLine : public QObject, public QGraphicsItem
  {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    
  public:
    ViewLine(const QLineF& line);
    virtual ~ViewLine();
    
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint(QPainter* painter, 
                       const QStyleOptionGraphicsItem* option, 
                       QWidget* widget);
    
    QLineF mapLineToScene(const QLineF& line)
    {
      QPointF p1 = line.p1(), p2 = line.p2();
      p1 = mapToScene(p1), p2 = mapToScene(p2);
      return QLineF(p1, p2);
    }
    
  signals:
    void changed(const QLineF& line);
    
  private:
    void updateLine(const QLineF& line);
    void updateLine_prim(const QLineF& line);
    
    // BEGIN -- Move Event
    
  protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    
  private:
    void resetMoveState();
    void dispatchMoveEvent(QGraphicsSceneMouseEvent* event);
    
    enum MyMoveEventState
    {
      MY_EVT_NONE,
      MY_EVT_MOVE_LINE,
      MY_EVT_MOVE_PT0,
      MY_EVT_MOVE_PT1
    } m_MoveState;
    
    // END   -- Move Event
    
  private:
    QLineF m_Line;
    QRectF m_EndPt0, m_EndPt1;
    QPainterPath m_BoundingPath;
  };
  
}
