#pragma once

#include "ViewModelLine.h"
#include "ViewLine.h"

#include <QObject>

namespace Morf
{
  
  class LineController : protected QObject
  {
    Q_OBJECT
    
  public:
    LineController();
    virtual ~LineController();
    
    ViewModelLinePair* addLine(const QLineF& line0, const QLineF& line1);
    void removeLast();
    int count()const{ return m_Lines.size(); }
    
    void getLines(QList<QLineF>& lines0, QList<QLineF>& lines1);
    
  private:
    QList<ViewModelLinePair*> m_Lines;
    
  };
}
