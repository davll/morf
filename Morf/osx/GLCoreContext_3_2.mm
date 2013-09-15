#include "GLCoreContext_3_2.h"

#include <QList>
#include <QVector>
#include <QDebug>

#import <OpenGL/OpenGL.h>
#import <Cocoa/Cocoa.h>

namespace Morf
{
  
  GLCoreContext_3_2::GLCoreContext_3_2
  (const QGLFormat& format, QPaintDevice* device)
    : QGLContext(format, device)
  {
  }
  
  GLCoreContext_3_2::GLCoreContext_3_2(const QGLFormat &format)
    : QGLContext(format)
  {
  }
  
  void* GLCoreContext_3_2::chooseMacVisual(GDHandle handle)
  {
    QGLFormat fmt = format();
    QList<NSOpenGLPixelFormatAttribute> attr;
    
    attr.append(NSOpenGLPFAAccelerated);
    
    if(fmt.doubleBuffer())
      attr.append(NSOpenGLPFADoubleBuffer);
    
    if(fmt.accum())
    {
      attr.append(NSOpenGLPFAAccumSize);
      attr.append(fmt.accumBufferSize());
    }
    
    if(fmt.depth())
    {
      attr.append(NSOpenGLPFADepthSize);
      attr.append(fmt.depthBufferSize());
    }
    
    if(fmt.stencil())
    {
      attr.append(NSOpenGLPFAStencilSize);
      attr.append(fmt.stencilBufferSize());
    }
    
    if(fmt.sampleBuffers())
    {
      attr.append(NSOpenGLPFAMultisample);
      attr.append(NSOpenGLPFASampleBuffers);
      attr.append(1);
      attr.append(NSOpenGLPFASamples);
      attr.append(fmt.samples());
    }  
    
    if(fmt.profile() == QGLFormat::CoreProfile)
    {
      attr.append(NSOpenGLPFAOpenGLProfile);
      attr.append(NSOpenGLProfileVersion3_2Core);
    }
    
    // ending
    
    attr.append(0);
    
    // generate array
    
    QVector<NSOpenGLPixelFormatAttribute> array = attr.toVector();
    
    return [[NSOpenGLPixelFormat alloc] initWithAttributes:array.data()];
  }
  
}
