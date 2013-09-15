#include <QApplication>

#include "AppController.h"

//
int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  
  Morf::AppController appController;
  
  return app.exec();
}
