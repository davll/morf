#include "ViewScene.h"
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMimeData>
#include <QDebug>
#include <QUrl>

namespace Morf
{
  
  ViewPixScene::ViewPixScene(QObject* parent)
    :QGraphicsScene(parent), m_ImgItem(NULL)
  {
    resetDropState(); // Reset Drop State
    m_LineToAdd = NULL; // Reset Line State
  }
  
  ViewPixScene::~ViewPixScene()
  {
    if(m_ImgItem)
      delete m_ImgItem;
  }
  
  void ViewPixScene::setPixmap(const QPixmap& pixmap)
  {
    foreach(QGraphicsView* view, this->views())
      view->setVisible(false);
    
    if(!m_ImgItem)
    {
      m_ImgItem = this->addPixmap(pixmap);
      m_ImgItem->setFlag(QGraphicsItem::ItemIsSelectable, false);
      m_ImgItem->setFlag(QGraphicsItem::ItemIsMovable, false);
      m_ImgItem->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
      m_ImgItem->setZValue(-1.0);
    }
    else
      m_ImgItem->setPixmap(pixmap);
    
    this->setSceneRect(pixmap.rect());
    
    foreach(QGraphicsView* view, this->views())
      view->setVisible(true);
  }
  
  // BEGIN -- Mouse Events
  
  void ViewPixScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
  {
    if(m_LineToAdd)
    {
      updateLineToAdd(mouseEvent);
      mouseEvent->accept();
    }
    else
      QGraphicsScene::mouseMoveEvent(mouseEvent);
  }
  
  void ViewPixScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
  {
    QGraphicsScene::mousePressEvent(mouseEvent);
    
    if(!mouseEvent->isAccepted())
    {
      beginLineToAdd(mouseEvent);
      mouseEvent->accept();
    }
  }
  
  void ViewPixScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
  {
    if(m_LineToAdd)
    {
      endLineToAdd(mouseEvent);
      mouseEvent->accept();
    }
    else
      QGraphicsScene::mouseReleaseEvent(mouseEvent);
  }
  
  void ViewPixScene::updateLineToAdd(QGraphicsSceneMouseEvent * mouseEvent)
  {
    QLineF line = m_LineToAdd->line();
    QPointF pos = mouseEvent->scenePos();
    
    m_LineToAdd->setLine(QLineF(line.p1(), pos));
  }
  
  void ViewPixScene::beginLineToAdd(QGraphicsSceneMouseEvent * mouseEvent)
  {
    QPen pen(QBrush(Qt::red), 3);
    QPointF pos = mouseEvent->scenePos();
    m_LineToAdd = this->addLine(QLineF(pos, pos), pen);
  }
  
  void ViewPixScene::endLineToAdd(QGraphicsSceneMouseEvent * mouseEvent)
  {
    updateLineToAdd(mouseEvent);
    
    // Emit Signal
    emit action_LineToAdd(m_LineToAdd->line());
    
    delete m_LineToAdd;
    m_LineToAdd = NULL; // Reset Line State
  }
  
  // END   -- 
  
  // BEGIN -- Drag Drop Events
  
  void ViewPixScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
  {
    if(checkAndAccept(event))
      setDropState();
  }
  
  void ViewPixScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
  {
    event->accept();
  }
  
  void ViewPixScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
  {
    event->accept();
    resetDropState();
    // BUG: It seems that dragLeaveEvent is not received when 
    //      moving one icon out of the window
  }
  
  void ViewPixScene::dropEvent(QGraphicsSceneDragDropEvent * event)
  {
    if(checkAndAccept(event))
    {
      QUrl url = extractUrl(event);
      //qDebug() << url.path();
      //qDebug() << url.toLocalFile();
      
      // Emit Signal
      emit action_DropImageUrl(url.toLocalFile());
      
      resetDropState();
    }
  }
  
  bool ViewPixScene::checkAndAccept(QGraphicsSceneDragDropEvent* event)
  {
    const QMimeData* mime = event->mimeData();
    bool accepted = false;
    
    if(mime->hasUrls())
    {
      QList<QUrl> urls = mime->urls();
      if(urls.size()==1)
        accepted = true;
    }
    
    if(accepted)
      event->setDropAction(Qt::CopyAction);
    
    return accepted;
  }
  
  QUrl ViewPixScene::extractUrl(QGraphicsSceneDragDropEvent* event)
  {
    const QMimeData* mime = event->mimeData();
    return mime->urls().first();
  }
  
  void ViewPixScene::setDropState()
  {
    //this->setBackgroundBrush(QBrush(Qt::cyan));
  }
  
  void ViewPixScene::resetDropState()
  {
    //this->setBackgroundBrush(QBrush(Qt::white));
  }
  
  // END   -- Drag Drop Events
  
}
