#pragma once

#include "OutputGLView.h"
#include "ui_OutputViewDialog.h"

#include <QObject>
#include <QList>
#include <QVector>
#include <QLineF>
#include <QPixmap>
#include <QImage>
#include <QWidget>
#include <QDialog>

namespace Morf
{
  
  class OutputController : public QObject
  {
    Q_OBJECT
  
  public:
    OutputController
    (const QList<QLineF>& line0, const QList<QLineF>& line1,
     const QPixmap& pixmap0, const QPixmap& pixmap1, QWidget* parent);
    
    virtual ~OutputController();
    
    void exec();
    
  private slots:
    void param_P_changed(double v);
    void param_A_changed(double v);
    void param_B_changed(double v);
    void slider_T_changed(int v);
    void slider_RangeChanged(int minv, int maxv);
    
    void action_ExportImage();
    
  private:
    void updateParamT(int v, int minv, int maxv);
    
  private:
    QDialog m_DialogWindow;
    Ui_Dialog m_DialogControls;
    OutputGLView* m_OutputView;
    
    int m_T_intMin, m_T_intMax, m_T_intValue;
    qreal m_Param_T;
    
  };
  
}
