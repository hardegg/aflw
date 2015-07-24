#include <QtGui/QApplication>
#include <Quarter/Quarter.h>
#include <Inventor/SoInteraction.h>

using namespace SIM::Coin3D::Quarter;

#include <iostream>
#include <cstdlib>
#include <string>

#include "main_window.h"

int main(int argc, char *argv[])
{
  //if (argc != 2)
  //{
  //   std::cerr << "usage: " << argv[0] << " inventor_file" << std::endl;
  //   return EXIT_FAILURE;
  //}

  QApplication app(argc, argv);
    app.setOrganizationName("ICG");
  app.setOrganizationDomain("icg.tugraz.at");
  app.setApplicationName("PoseAnnotationGuiSQL");
  
  const bool initCoin = true;
  Quarter::init(initCoin);
  SoInteraction::init();

  MainWindow * mainWindow = new MainWindow();
  mainWindow->showMaximized();

  app.exec();
  
  // Clean up resources.
  delete mainWindow;
  Quarter::clean();

  return EXIT_SUCCESS;
}
