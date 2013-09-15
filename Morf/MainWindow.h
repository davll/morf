#pragma once

#include <QMainWindow>

#include "ui_MainWindow.h"
#include "ViewScene.h"

namespace Morf
{
  
  class AppController;
  
  class MainWindow : public QMainWindow, public Ui_MainWindow
  {
    Q_OBJECT
    
  public:
    MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();
    
    ViewPixScene* getImgScene0() { return &m_ImgScene0; }
    ViewPixScene* getImgScene1() { return &m_ImgScene1; }
    
  private:
    ViewPixScene m_ImgScene0;
    ViewPixScene m_ImgScene1;
    
  };
  
}
