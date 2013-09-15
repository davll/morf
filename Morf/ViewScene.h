#pragma once

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QMap>

namespace Morf
{
  
  class ViewPixScene : public QGraphicsScene
  {
    Q_OBJECT
    
  public:
    ViewPixScene(QObject* parent = 0);
    virtual ~ViewPixScene();
    
    void setPixmap(const QPixmap& pixmap);
    
  signals:
    void action_DropImageUrl(const QString& path);
    void action_LineToAdd(const QLineF& line);
    
  private:
    void dropOneFile(const QUrl& url);
    
  private:
    QGraphicsPixmapItem* m_ImgItem;
    
    // BEGIN -- Mouse Events
    
  protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    
  private:
    void updateLineToAdd(QGraphicsSceneMouseEvent * mouseEvent);
    void beginLineToAdd(QGraphicsSceneMouseEvent * mouseEvent);
    void endLineToAdd(QGraphicsSceneMouseEvent * mouseEvent);
    
  private:
    QGraphicsLineItem* m_LineToAdd;
    
    // END   -- 
    
    // BEGIN -- Drag Drop Events
    
  protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent * event);
    
  private:
    bool checkAndAccept(QGraphicsSceneDragDropEvent* event);
    QUrl extractUrl(QGraphicsSceneDragDropEvent* event);
    void setDropState();
    void resetDropState();
    
    // END   -- Drag Drop Events
    
  };
  
}
