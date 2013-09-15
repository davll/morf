#pragma once

#include <QLineF>
#include <QVector>
#include <QList>
#include <QImage>
#include <QGLWidget>

namespace Morf
{
  
  class OutputGLView : public QGLWidget
  {
    Q_OBJECT
    
  public:
    enum { MAX_LINE_COUNT = 64 };
    
  public:
    OutputGLView(const QImage& img0, const QImage& img1,
                 QWidget* parent = 0);
    virtual ~OutputGLView();
    
    void setLines(const QList<QLineF>& lines0, const QList<QLineF>& lines1);
    
    void setT(qreal t);
    void setP(qreal val);
    void setA(qreal val);
    void setB(qreal val);
    
    QImage exportOutput();
    
  protected:
    virtual void initializeGL();
    //virtual void resizeGL(int w, int h);
    virtual void paintGL();
    
  private:
    static QGLFormat preferedFormat();
    
    // update content
    
    void updateContent();
    
    // dirty state;
    enum DirtyMask{
      DIRTY_LINES0 = 0x8,
      DIRTY_LINES1 = 0x10,
      DIRTY_OUTBUFFER_DATA = 0x20,
      DIRTY_T = 0x100,
      DIRTY_PARAM = 0x200,
      DIRTY_NULL = 0x0
    };
    inline void resetDirty() { m_Dirty = 0; }
    inline void setDirty(DirtyMask mask) { m_Dirty |= (quint32)mask; }
    inline quint32 dirty(){ return m_Dirty; }
    quint32 m_Dirty;
    
    // safe release
    inline void safeDeleteTexture(GLuint& obj)
    {
      if(obj)
        this->deleteTexture(obj);
      obj = 0;
    }
    
  private:
    // Parameters
    GLfloat m_T;
    GLfloat m_P, m_A, m_B;
    
    // Qt Resources
    QImage m_Image0;
    QImage m_Image1;
    QRect m_OutputRect;
    QRect m_RenderRect;
    
    // layout: vec4( x0, y0, x1, y1 )
    // coordinate: Output Clip Space (Y inverted)
    QVector<GLfloat> m_Lines0;
    QVector<GLfloat> m_Lines1;
    QVector<GLfloat> m_LinesX;
    GLuint m_Lines0UBO;
    GLuint m_Lines1UBO;
    GLuint m_LinesXUBO;
    
    // Quad Vertex Buffer
    GLuint m_QuadVAO;
    GLuint m_QuadVBO;
    
    // Quad GLSL Vertex Shader
    GLuint m_QuadVS;
    
    // GLSL Morph Program
    GLuint m_MorphProgram;
    
    // GLSL View Program
    GLuint m_ViewProgram;
    
    // Input Textures
    GLuint m_Texture0;
    GLuint m_Texture1;
    
    // Output Framebuffer
    GLuint m_OutTex;
    GLuint m_OutFBO;
    
    // note: don't worry about resource deallocation on desctructor, 
    //       since QGLContext will automatically release resources
  };
  
}
