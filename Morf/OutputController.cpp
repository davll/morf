#include "OutputController.h"

#include <QApplication>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QDebug>

namespace Morf
{
  
  OutputController::OutputController
  (const QList<QLineF>& line0, const QList<QLineF>& line1,
   const QPixmap& pixmap0, const QPixmap& pixmap1, QWidget* parent)
    : QObject(), 
      m_DialogWindow(parent)
  {
    
    // prepare ui
    
    m_DialogControls.setupUi(&m_DialogWindow);
    
    // initialize view
    m_OutputView = new OutputGLView(pixmap0.toImage(), pixmap1.toImage(), 
                                    m_DialogControls.outputArea);
    m_OutputView->setLines(line0, line1);
    
    {
      QLayout* layout = new QVBoxLayout();
      layout->addWidget(m_OutputView);
      m_DialogControls.outputArea->setLayout(layout);
    }
    
    // connect ui binding
    
    m_OutputView->setP(m_DialogControls.inputP->value());
    m_OutputView->setA(m_DialogControls.inputA->value());
    m_OutputView->setB(m_DialogControls.inputB->value());
    
    m_T_intMin = m_DialogControls.inputTime->minimum();
    m_T_intMax = m_DialogControls.inputTime->maximum();
    m_T_intValue = m_DialogControls.inputTime->value();
    this->updateParamT(m_T_intValue, m_T_intMin, m_T_intMax);
    
    connect(m_DialogControls.inputP, SIGNAL(valueChanged(double)), 
            SLOT(param_P_changed(double)));
    connect(m_DialogControls.inputA, SIGNAL(valueChanged(double)), 
            SLOT(param_A_changed(double)));
    connect(m_DialogControls.inputB, SIGNAL(valueChanged(double)), 
            SLOT(param_B_changed(double)));
    
    connect(m_DialogControls.inputTime, SIGNAL(valueChanged(int)),
            SLOT(slider_T_changed(int)));
    connect(m_DialogControls.inputTime, SIGNAL(rangeChanged(int,int)),
            SLOT(slider_RangeChanged(int,int)));
    
    connect(m_DialogControls.btnExportImage, SIGNAL(clicked()),
            SLOT(action_ExportImage()));
    
    
  }
  
  OutputController::~OutputController()
  {
  }
  
  void OutputController::exec()
  {
    m_DialogWindow.exec();
  }
  
  void OutputController::param_P_changed(double v)
  {
    m_OutputView->setP(v);
    m_OutputView->update();
  }
  
  void OutputController::param_A_changed(double v)
  {
    m_OutputView->setA(v);
    m_OutputView->update();
  }
  
  void OutputController::param_B_changed(double v)
  {
    m_OutputView->setB(v);
    m_OutputView->update();
  }
  
  void OutputController::slider_T_changed(int v)
  {
    m_T_intValue = v;
    updateParamT(v, m_T_intMin, m_T_intMax);
  }
  
  void OutputController::slider_RangeChanged(int minv, int maxv)
  {
    m_T_intMin = minv;
    m_T_intMax = maxv;
    updateParamT(m_T_intValue, minv, maxv);
  }
  
  void OutputController::updateParamT(int v, int minv, int maxv)
  {
    int range = maxv - minv;
    int val = v - minv;
    
    m_Param_T = (qreal) val / (qreal) range;
    
    m_OutputView->setT(m_Param_T);
    m_OutputView->update();
  }
  
  void OutputController::action_ExportImage()
  {
    QFileDialog dialog(&m_DialogWindow, "Save Image");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("png");
    
    if(dialog.exec())
    {
      QStringList files = dialog.selectedFiles();
      m_OutputView->exportOutput().save(files.first());
    }
  }
  
}
