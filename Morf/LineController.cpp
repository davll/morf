#include "LineController.h"

namespace Morf
{
  
  LineController::LineController()
  {
  }
  
  LineController::~LineController()
  {
  }
  
  ViewModelLinePair* LineController::addLine(const QLineF& line0, 
                                             const QLineF& line1)
  {
    //int row = m_Lines.size();
    //this->beginInsertRows(QModelIndex(), row, row);
    
    ViewModelLinePair* obj = new ViewModelLinePair(line0, line1);
    obj->setParent(this);
    m_Lines.append(obj);
    
    //this->connect(obj, SIGNAL(line0Changed(QLineF)), 
    //              SLOT(lineChanged(QLineF)));
    //this->connect(obj, SIGNAL(line1Changed(QLineF)), 
    //              SLOT(lineChanged(QLineF)));
    
    //this->endInsertRows();
    
    return obj;
  }
  
  void LineController::removeLast()
  {
    ViewModelLinePair* obj = m_Lines.takeLast();
    obj->deleteLater();
  }
  
  void LineController::getLines(QList<QLineF>& lines0, QList<QLineF>& lines1)
  {
    lines0.clear();
    lines1.clear();
    
    foreach(ViewModelLinePair* obj, m_Lines)
    {
      lines0.append( obj->line0() );
      lines1.append( obj->line1() );
    }
  }
  
}
