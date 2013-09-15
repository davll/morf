#pragma once

#include <QGLContext>

namespace Morf
{
  
  class GLCoreContext_3_2 : public QGLContext
  {
  public:
    GLCoreContext_3_2(const QGLFormat& format, QPaintDevice* device);
    GLCoreContext_3_2(const QGLFormat &format);
    
  protected:
    virtual void* chooseMacVisual(GDHandle handle);
    
  private:
    
  };
  
}
