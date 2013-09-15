#include "ViewModelLine.h"
#include "ViewLine.h"

#include <QString>
#include <QTextStream>
#include <QDebug>

namespace Morf
{
  
  ViewModelLinePair::ViewModelLinePair(QLineF line0, QLineF line1)
    : m_Line0(line0), m_Line1(line1)
  {
  }
  
  ViewModelLinePair::~ViewModelLinePair()
  {
  }
  
  void ViewModelLinePair::setLine0(const QLineF& line)
  {
    //qDebug() << "set0";
    m_Line0 = line;
    emit line0Changed(m_Line0);
  }
  
  void ViewModelLinePair::setLine1(const QLineF& line)
  {
    //qDebug() << "set1";
    m_Line1 = line;
    emit line1Changed(m_Line1);
  }
  
  QVariant ViewModelLinePair::data() const
  {
    QString result;
    QTextStream stream(&result);
    
    qreal l1x1 = m_Line0.p1().x(), l1x2 = m_Line0.p2().x();
    qreal l1y1 = m_Line0.p1().y(), l1y2 = m_Line0.p2().y();
    qreal l2x1 = m_Line1.p1().x(), l2x2 = m_Line1.p2().x();
    qreal l2y1 = m_Line1.p1().y(), l2y2 = m_Line1.p2().y(); 
    
    stream <<"{("<<l1x1<<","<<l1y1<<") => ("<<l1x2<<","<<l1y2<<")}:";
    stream <<"{("<<l2x1<<","<<l2y1<<") => ("<<l2x2<<","<<l2y2<<")}";
    
    return QVariant(result);
  }
  
}
