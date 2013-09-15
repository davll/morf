#pragma once

#include <QObject>
#include <QLineF>
#include <QVariant>

namespace Morf
{
  
  class ViewLine;
  
  class ViewModelLinePair : public QObject
  {
    Q_OBJECT
    
  public:
    ViewModelLinePair(QLineF line0, QLineF line1);
    virtual ~ViewModelLinePair();
    
  signals:
    void line0Changed(const QLineF& line);
    void line1Changed(const QLineF& line);
    
  public:
    QLineF line0() const { return m_Line0; }
    QLineF line1() const { return m_Line1; }
    
  public slots:
    void setLine0(const QLineF& line);
    void setLine1(const QLineF& line);
    
  public:
    QVariant data() const;
    
  private:
    QLineF m_Line0;
    QLineF m_Line1;
  };
  
}
