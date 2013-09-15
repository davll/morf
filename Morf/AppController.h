#pragma once

#include "MainWindow.h"
#include "IdManager.h"
#include "LineController.h"

#include <QObject>
#include <QMap>

namespace Morf
{
  
  class ViewPixScene;
  
  class AppController : public QObject
  {
    Q_OBJECT
    
  public:
    AppController();
    virtual ~AppController();
    
  private slots:
    void scene0_addLine(const QLineF& line);
    void scene1_addLine(const QLineF& line);
    void scene0_addPixmap(const QString& path);
    void scene1_addPixmap(const QString& path);
    void model_addLine(const QLineF& line0, const QLineF& line1);
    void model_removeLastLine();
    
    void action_openOutput();
    void action_loadImage0();
    void action_loadImage1();
    void action_openFile();
    void action_saveFile();
    
  private:
    bool _loadFile(const QString& path);
    
  private:
    MainWindow m_MainWindow;
    ViewPixScene *m_Scene0, *m_Scene1;
    LineController m_LineController;
    QPixmap m_Img0, m_Img1;
    
  };
  
}
