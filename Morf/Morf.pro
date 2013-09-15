QT			= core gui opengl
CONFIG		+= qt warn_on
TEMPLATE	= app
TARGET		= Morf

#macx.LIBS = -framework QuartzCore

OBJECTS_DIR	= ../Morf-obj
RCC_DIR		= ../Morf-obj
UI_DIR		= ../Morf-obj
MOC_DIR		= ../Morf-obj
DESTDIR		= ../Morf-bin
DLLDESTDIR	= ../Morf-bin

# Main Source
SOURCES		= Main.cpp AppController.cpp
HEADERS		= AppController.h IdManager.h
FORMS		= ''
RESOURCES	= MainResources.qrc
DEPENDPATH  = shaders/*.vert shaders/*.frag

# Main Window
SOURCES		+= MainWindow.cpp
HEADERS		+= MainWindow.h
FORMS		+= MainWindow.ui

# View Scene
SOURCES		+= ViewScene.cpp
HEADERS		+= ViewScene.h

# View Line
SOURCES		+= ViewLine.cpp
HEADERS		+= ViewLine.h

# View-Model Line
SOURCES		+= ViewModelLine.cpp
HEADERS		+= ViewModelLine.h

# Line Controller
SOURCES		+= LineController.cpp
HEADERS		+= LineController.h

# Output Dialog
SOURCES		+= OutputController.cpp
HEADERS		+= OutputController.h
FORMS		+= OutputViewDialog.ui

# Output OpenGL View
SOURCES		+= OutputGLView.cpp
HEADERS		+= OutputGLView.h

# GLEW
SOURCES     += glew/glew.c
HEADERS     += glew/glew.h
DEFINES     += GLEW_STATIC

# Mac OpenGL Fix
macx {
  OBJECTIVE_SOURCES += osx/GLCoreContext_3_2.mm
  HEADERS   += osx/GLCoreContext_3_2.h
  LIBS += -framework Cocoa -framework OpenGL
}

#
#macx {
#  ICON = ../ak47/rc/AppIcon.icns
#}
