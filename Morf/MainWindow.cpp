#include "MainWindow.h"

namespace Morf
{
  
  MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_ImgScene0(), m_ImgScene1()
  {
    // Initialize UI
    setupUi(this);
    
    // Assign Scenes
    imageView0->setScene(&m_ImgScene0);
    imageView1->setScene(&m_ImgScene1);
    
  }
  
  MainWindow::~MainWindow()
  {
  }
  
}
