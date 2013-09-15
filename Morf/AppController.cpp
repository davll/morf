#include "AppController.h"
#include "ViewLine.h"
#include "ViewScene.h"
#include "ViewModelLine.h"
#include "OutputController.h"

#include <QUrl>
#include <QPixmap>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

namespace Morf
{
  
  AppController::AppController()
    : m_MainWindow()
  {
    m_Scene0 = m_MainWindow.getImgScene0();
    m_Scene1 = m_MainWindow.getImgScene1();
    
    QObject::connect(m_Scene0, SIGNAL(action_LineToAdd(QLineF)), 
                     this, SLOT(scene0_addLine(QLineF)));
    
    QObject::connect(m_Scene0, SIGNAL(action_DropImageUrl(QString)),
                     this, SLOT(scene0_addPixmap(QString)));
    
    QObject::connect(m_Scene1, SIGNAL(action_LineToAdd(QLineF)), 
                     this, SLOT(scene1_addLine(QLineF)));
    
    QObject::connect(m_Scene1, SIGNAL(action_DropImageUrl(QString)),
                     this, SLOT(scene1_addPixmap(QString)));
    
    QObject::connect(m_MainWindow.actionUndo, SIGNAL(triggered()),
                     this, SLOT(model_removeLastLine()));
    m_MainWindow.actionUndo->setEnabled(false);
    
    QObject::connect(m_MainWindow.actionOutput, SIGNAL(triggered()), 
                     this, SLOT(action_openOutput()));
    
    connect(m_MainWindow.actionLoad_Image_0, SIGNAL(triggered()), 
            SLOT(action_loadImage0()));
    
    connect(m_MainWindow.actionLoad_Image_1, SIGNAL(triggered()), 
            SLOT(action_loadImage1()));
    
    connect(m_MainWindow.actionOpen, SIGNAL(triggered()), 
            SLOT(action_openFile()));
    
    connect(m_MainWindow.actionSave, SIGNAL(triggered()), 
            SLOT(action_saveFile()));
    
    m_MainWindow.show();
  }
  
  AppController::~AppController()
  {
    
  }
  
  void AppController::scene0_addLine(const QLineF& line)
  {
    model_addLine(line, line);
  }
  
  void AppController::scene1_addLine(const QLineF& line)
  {
    model_addLine(line, line);
  }
  
  void AppController::scene0_addPixmap(const QString& path)
  {
    if ( _loadFile(path) )
    {
    }
    else if ( m_Img0.load(path) )
    {
      m_Scene0->setPixmap(m_Img0);
    }
  }
  
  void AppController::scene1_addPixmap(const QString& path)
  {
    if ( _loadFile(path) )
    {
    }
    else if ( m_Img1.load(path) )
    {
      m_Scene1->setPixmap(m_Img1);
    }
  }
  
  void AppController::model_addLine(const QLineF& line0, const QLineF& line1)
  {
    ViewModelLinePair* lineModel = m_LineController.addLine(line0, line1);
    ViewLine* vline0 = new ViewLine(line0);
    ViewLine* vline1 = new ViewLine(line1);
    vline0->setParent(lineModel);
    vline1->setParent(lineModel);
    m_Scene0->addItem(vline0);
    m_Scene1->addItem(vline1);
    
    QObject::connect(vline0, SIGNAL(changed(QLineF)), 
                     lineModel, SLOT(setLine0(QLineF)));
    QObject::connect(vline1, SIGNAL(changed(QLineF)), 
                     lineModel, SLOT(setLine1(QLineF)));
    
    m_MainWindow.actionUndo->setEnabled(true);
  }
  
  void AppController::model_removeLastLine()
  {
    m_LineController.removeLast();
    
    if(m_LineController.count() == 0)
      m_MainWindow.actionUndo->setEnabled(false);
  }
  
  void AppController::action_openOutput()
  {
    QList<QLineF> line0, line1;
    m_LineController.getLines(line0, line1);
    
    if(m_Img0.isNull() || m_Img1.isNull())
    {
      QMessageBox::critical(&m_MainWindow, "Alert", 
                            "images are required");
      return;
    }
    
    if(line0.count() < 1 || line1.count() < 1)
    {
      QMessageBox::critical(&m_MainWindow, "Alert", 
                            "matching lines are required");
      return;
    }
    
    if(line0.count() != line1.count())
    {
      QMessageBox::critical(&m_MainWindow, "Alert", 
                            "the number of lines of image 0 must be the same"
                            " as image 1's");
      return;
    }
    
    //if(m_Img0.size() != m_Img1.size())
    //{
    //  QMessageBox::critical(&m_MainWindow, "Alert",
    //                        "the sizes of the two images should be the same");
    //  return;
    //}
    
    if(line0.count() > OutputGLView::MAX_LINE_COUNT)
    {
      
      QString fmt("number of line pairs should be no more than %1");
      
      QMessageBox::critical(&m_MainWindow, "Alert",
                            fmt.arg((int)OutputGLView::MAX_LINE_COUNT));
      return;
    }
    
    OutputController outputController(line0, line1, m_Img0, m_Img1,
                                      &m_MainWindow);
    outputController.exec();
  }
  
  void AppController::action_loadImage0()
  {
    QFileDialog dialog(&m_MainWindow, "Open Image 0");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
    
    if(dialog.exec())
    {
      QStringList files = dialog.selectedFiles();
      
      if ( m_Img0.load(files.first()) )
        m_Scene0->setPixmap(m_Img0);
      
    }
  }
  
  void AppController::action_loadImage1()
  {
    QFileDialog dialog(&m_MainWindow, "Open Image 1");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
    
    if(dialog.exec())
    {
      QStringList files = dialog.selectedFiles();
      
      if ( m_Img1.load(files.first()) )
        m_Scene1->setPixmap(m_Img1);
    }
  }
  
  void AppController::action_openFile()
  {
    QFileDialog dialog(&m_MainWindow, "Open File");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter("*.dat");
    
    if(dialog.exec())
    {
      QString path = dialog.selectedFiles().first();
      _loadFile(path);
    }
  }
  
  void AppController::action_saveFile()
  {
    QFileDialog dialog(&m_MainWindow, "Save File");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("dat");
    dialog.setNameFilter("*.dat");
    
    if(dialog.exec())
    {
      QString path = dialog.selectedFiles().first();
      QFile file(path);
      file.open(QFile::WriteOnly);
      QDataStream stream(&file);
      
      QList<QLineF> lines0, lines1;
      m_LineController.getLines(lines0, lines1);
      
      int count = lines0.count();
      stream << count;
      for(int i=0; i<count; ++i)
      {
        const QLineF& l0 = lines0[i];
        const QLineF& l1 = lines1[i];
        stream << l0.x1() << l0.y1() << l0.x2() << l0.y2();
        stream << l1.x1() << l1.y1() << l1.x2() << l1.y2();
      }
      
      stream << m_Img0 << m_Img1;
    }
  }
  
  bool AppController::_loadFile(const QString& path)
  {
    if ( !path.endsWith(".dat") )
      return false;
    
    QFile file(path);
    file.open(QFile::ReadOnly);
    QDataStream stream(&file);
    
    while(m_LineController.count() > 0)
      model_removeLastLine();
    
    int count;
    stream >> count;
    
    qreal x1, y1, x2, y2;
    for(int i=0; i<count; ++i)
    {
      stream >> x1 >> y1 >> x2 >> y2;
      QLineF line0(x1, y1, x2, y2);
      stream >> x1 >> y1 >> x2 >> y2;
      QLineF line1(x1, y1, x2, y2);
      model_addLine(line0, line1);
    }
    
    stream >> m_Img0 >> m_Img1;
    m_Scene0->setPixmap(m_Img0);
    m_Scene1->setPixmap(m_Img1);
    
    return true;
  }
  
}
