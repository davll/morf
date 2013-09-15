#if defined(__APPLE__)
#  include <OpenGL/gl3.h>
#  include "osx/GLCoreContext_3_2.h"
#  define MORF_GL_CORE_3_2
#endif

#ifndef MORF_GL_CORE_3_2
#  include "glew/glew.h"
#endif

#include "OutputGLView.h"

#include <QDebug>
#include <QGLFormat>
#include <QResource>

#include <cassert>
#include <cmath>

namespace Morf
{
  
  OutputGLView::OutputGLView
  (const QImage& img0, const QImage& img1, QWidget* parent)
#ifdef MORF_GL_CORE_3_2
    : QGLWidget(new GLCoreContext_3_2(preferedFormat()), parent)
#else
    : QGLWidget(preferedFormat(), parent)
#endif
  {
    assert(!img0.isNull());
    assert(!img1.isNull());
    //assert(img0.size() == img1.size());
    
    m_Image0 = img0;
    m_Image1 = img1;
    m_OutputRect = m_Image0.rect().united(m_Image1.rect());
    
    m_RenderRect = m_OutputRect;
    
    if(m_RenderRect.width() < m_RenderRect.height())
      m_RenderRect.setWidth(m_RenderRect.height());
    else if(m_RenderRect.width() > m_RenderRect.height())
      m_RenderRect.setHeight(m_RenderRect.width());
    
    m_T = 0.0;
    m_P = 0.0;
    m_A = 1.0;
    m_B = 2.0;
    
    resetDirty();
  }
  
  OutputGLView::~OutputGLView()
  {
  }
  
  namespace
  {
    inline void _copyLines
    (const QList<QLineF>& src, QVector<GLfloat>& dst, 
     const QRect& srcRect, const QRect& dstRect)
    {
      int n = src.size();
      
      dst.resize(4*n);
      
      QSize ss = srcRect.size();
      float scx = ss.width() * 0.5;
      float scy = ss.height() * 0.5;
      
      QSize ds = dstRect.size();
      float dsx = 2.0 / ds.width();
      float dsy = 2.0 / ds.height();
      
      for(int i=0; i<n; ++i)
      {
        const QLineF& l = src[i];
        dst[4*i+0] = (l.x1() - scx) * dsx;
        dst[4*i+1] = (scy - l.y1()) * dsy;
        dst[4*i+2] = (l.x2() - scx) * dsx;
        dst[4*i+3] = (scy - l.y2()) * dsy;
      }
    }
  }
  
  void OutputGLView::setLines
  (const QList<QLineF>& lines0, const QList<QLineF>& lines1)
  {
    assert(lines0.size() > 0);
    assert(lines1.size() > 0);
    assert(lines0.size() == lines1.size());
    
    _copyLines(lines0, m_Lines0, m_Image0.rect(), m_RenderRect);
    _copyLines(lines1, m_Lines1, m_Image1.rect(), m_RenderRect);
    
    setDirty(DIRTY_LINES0);
    setDirty(DIRTY_LINES1);
    
    this->update();
  }
  
  void OutputGLView::setT(qreal t)
  {
    m_T = t;
    setDirty(DIRTY_T);
  }
  
  void OutputGLView::setP(qreal val)
  {
    m_P = val;
    setDirty(DIRTY_PARAM);
  }
  
  void OutputGLView::setA(qreal val)
  {
    m_A = val;
    setDirty(DIRTY_PARAM);
  }
  
  void OutputGLView::setB(qreal val)
  {
    m_B = val;
    setDirty(DIRTY_PARAM);
  }
  
  QImage OutputGLView::exportOutput()
  {
    QImage img(m_OutputRect.size(), QImage::Format_ARGB32);
    
    this->makeCurrent();
    
    // Update Content
    updateContent();
    
    glFlush();
    
    // Update Output Image
    
    glBindTexture(GL_TEXTURE_2D, m_OutTex);
    
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, 
                  GL_UNSIGNED_INT_8_8_8_8_REV, 
                  img.bits());
    
    // Reset Current Texture
    glBindTexture(GL_TEXTURE_2D, 0);
    
    this->doneCurrent();
    
    return img;
  }
  
  namespace
  {
    void create_quad_vbo(GLuint& vao, GLuint& vbo)
    {
      const GLfloat quad_v[] = {
        -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0
      };
      const GLsizei quad_velem = 2;
      const GLenum quad_vtype = GL_FLOAT;
      
      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);
      
      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(quad_v), quad_v, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, quad_velem, quad_vtype, GL_FALSE, 0, NULL);
      
      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
  }
  
  namespace
  {
    GLuint _compileShaderFile(GLenum shaderType, const QString& path)
    {
      QResource file(path);
      
      GLuint obj = glCreateShader(shaderType);
      
      const GLchar* codes[] = { (const GLchar*)file.data() };
      GLint lengths[] = { file.size() };
      
      glShaderSource(obj, 1, (const GLchar**)codes, lengths);
      glCompileShader(obj);
      
      GLint status;
      glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
      
      if(status == GL_TRUE)
        return obj;
      
      GLint info_length;
      glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &info_length);
      
      QByteArray info(info_length, 0);
      glGetShaderInfoLog(obj, info_length, NULL, info.data());
      
      qWarning() << "Shader compilation failed: \n" << QString(info);
      
      glDeleteShader(obj);
      
      return 0;
    }
    
    void _addShaderFile(GLuint program, GLenum shaderType, const QString& path)
    {
      GLuint shader = _compileShaderFile(shaderType, path);
      if(shader)
      {
        glAttachShader(program, shader);
        glDeleteShader(shader);
      }
    }
    
    void _linkProgram(GLuint program)
    {
      glLinkProgram(program);
      
      GLint result;
      glGetProgramiv(program, GL_LINK_STATUS, &result);
      if(result != GL_TRUE)
      {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &result);
        
        QByteArray info(result, 0);
        glGetProgramInfoLog(program, result, NULL, info.data());
        
        qWarning() << "Program linkage issue: \n" << QString(info);
      }
    }
    
  }
  
  void OutputGLView::initializeGL()
  {
#ifdef __GLEW_H__
    glewInit();
#endif
    
    qDebug()<<"GL: "<<(const char*)glGetString(GL_VERSION);
    qDebug()<<"GLSL: "<<(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    GLint num;
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &num);
    qDebug()<<"Max Frag. Uniform Components: " << num;
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &num);
    qDebug()<<"Max Frag. Uniform Blocks: " << num;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &num);
    qDebug()<<"Max Uniform Block Size: " << num;  
    
    // Init Input Textures
    m_Texture0 = this->bindTexture(m_Image0);
    m_Texture1 = this->bindTexture(m_Image1);
    glBindTexture(GL_TEXTURE_2D, m_Texture0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, m_Texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Create Quad Vertex Buffer
    create_quad_vbo(m_QuadVAO, m_QuadVBO);
    
    // Check GLSL Versions
    QString glslver = "150";
    
    // Create Quad Vertex Shader
    m_QuadVS = _compileShaderFile(GL_VERTEX_SHADER, 
                                  ":/main/shaders/quad_"+glslver+".vert");
    
    // Create GLSL Morph Program
    m_MorphProgram = glCreateProgram();
    glAttachShader(m_MorphProgram, m_QuadVS);
    _addShaderFile(m_MorphProgram, GL_FRAGMENT_SHADER, 
                   ":/main/shaders/morph_"+glslver+".frag");
    _linkProgram(m_MorphProgram);
    
    // Create GLSL View Program
    m_ViewProgram = glCreateProgram();
    glAttachShader(m_ViewProgram, m_QuadVS);
    _addShaderFile(m_ViewProgram, GL_FRAGMENT_SHADER, 
                   ":/main/shaders/view_"+glslver+".frag");
    _linkProgram(m_ViewProgram);
    
    // Create Output Framebuffer
    
    glGenTextures(1, &m_OutTex);
    glBindTexture(GL_TEXTURE_2D, m_OutTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 
                 m_OutputRect.width(), m_OutputRect.height(), 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenFramebuffers(1, &m_OutFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_OutFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, m_OutTex, 0);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
      switch(status)
      {
      case GL_FRAMEBUFFER_UNDEFINED:
        qDebug() << "GL_FRAMEBUFFER_UNDEFINED";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        qDebug() << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        qDebug() << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        qDebug() << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        qDebug() << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
        break;
      case GL_FRAMEBUFFER_UNSUPPORTED:
        qDebug() << "GL_FRAMEBUFFER_UNSUPPORTED";
        break;
      }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Uniform Buffer Object
    glGenBuffers(1, &m_Lines0UBO);
    glGenBuffers(1, &m_Lines1UBO);
    glGenBuffers(1, &m_LinesXUBO);
    
    GLsizei size = sizeof(GLfloat)*4*MAX_LINE_COUNT;
    
    glBindBuffer(GL_UNIFORM_BUFFER, m_Lines0UBO);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    
    glBindBuffer(GL_UNIFORM_BUFFER, m_Lines1UBO);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    
    glBindBuffer(GL_UNIFORM_BUFFER, m_LinesXUBO);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
  }
  
  void OutputGLView::paintGL()
  {
    updateContent();
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
    glViewport(0, 0, (GLsizei)width(), (GLsizei)height());
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(m_ViewProgram);
    
    glUniform2f(glGetUniformLocation(m_ViewProgram, "quad_scale"), 
                (GLfloat)m_OutputRect.width() / (GLfloat)width(), 
                (GLfloat)m_OutputRect.height() / (GLfloat)height());
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_OutTex);
    
    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glUseProgram(0);
  }
  
  QGLFormat OutputGLView::preferedFormat()
  {
    QGLFormat format(QGL::DoubleBuffer | 
                     QGL::NoAccumBuffer | 
                     QGL::NoDepthBuffer | 
                     QGL::NoOverlay | 
                     QGL::NoSampleBuffers | 
                     QGL::NoStencilBuffer | 
                     QGL::NoStereoBuffers );
    
#ifdef MORF_GL_CORE_3_2
    format.setVersion(3, 2); // For Mac OS X Lion
#endif
    format.setProfile(QGLFormat::CoreProfile);
    
    return format;
  }
  
  void OutputGLView::updateContent()
  {
    if (dirty())
    {
      GLsizei ubo_size_lines = sizeof(GLfloat) * m_Lines0.size();
      
      if (dirty() & DIRTY_LINES0)
      {
        glBindBuffer(GL_UNIFORM_BUFFER, m_Lines0UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, ubo_size_lines, m_Lines0.data());
        
        setDirty(DIRTY_T);
        setDirty(DIRTY_OUTBUFFER_DATA);
      }
      if (dirty() & DIRTY_LINES1)
      {
        glBindBuffer(GL_UNIFORM_BUFFER, m_Lines1UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, ubo_size_lines, m_Lines1.data());
        
        setDirty(DIRTY_T);
        setDirty(DIRTY_OUTBUFFER_DATA);
      }
      
      if (dirty() & DIRTY_PARAM)
      {
        setDirty(DIRTY_OUTBUFFER_DATA);
      }
      
      if (dirty() & DIRTY_T)
      {
        int n = m_Lines0.size();
        m_LinesX.resize(n);
        
        GLfloat t = (m_T);
        for(int i=0; i<n; ++i)
        {
          m_LinesX[i] = m_Lines0[i] + (m_Lines1[i] - m_Lines0[i]) * t;
        }
        
        glBindBuffer(GL_UNIFORM_BUFFER, m_LinesXUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, ubo_size_lines, m_LinesX.data());
        
        setDirty(DIRTY_OUTBUFFER_DATA);
      }
      
      // Reset Current Uniform Buffer
      glBindBuffer(GL_UNIFORM_BUFFER, 0);
      
      // Setup Shader Parameters
      
      int line_num = m_Lines0.size() / 4;
      GLfloat img0_w = m_Image0.width();
      GLfloat img0_h = m_Image0.height();
      GLfloat img1_w = m_Image1.width();
      GLfloat img1_h = m_Image1.height();
      GLfloat dst_w = m_OutputRect.width();
      GLfloat dst_h = m_OutputRect.height();
      GLfloat rnd_s = m_RenderRect.width();
      
      GLint texcoordScaleLoc = 
          glGetUniformLocation(m_MorphProgram, "texcoordScale");
      //GLint linesPQbarLoc = 
      //    glGetUniformLocation(m_MorphProgram, "linesPQbar");
      {
        GLuint bi = glGetUniformBlockIndex(m_MorphProgram, "linesPQbarBlock");
        glUniformBlockBinding(m_MorphProgram, bi, 1);
      }
      {
        GLuint bi = glGetUniformBlockIndex(m_MorphProgram, "linesPQBlock");
        glUniformBlockBinding(m_MorphProgram, bi, 0);
      }
      
      // Render
      
      if (dirty() & DIRTY_OUTBUFFER_DATA)
      {
        // Make Current Render Target
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_OutFBO);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        
        // Set Viewport
        glViewport(0, 0, (GLsizei)m_OutputRect.width(), 
                   (GLsizei)m_OutputRect.height());
        
        // Set Blending Operation
        glBlendColor(0.0, 0.0, 0.0, m_T);
        glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        
        // Use Morph Porgram
        glUseProgram(m_MorphProgram);
        
        // Use Quad Vertex Array Object
        glBindVertexArray(m_QuadVAO);
        
        // Set Render Scale
        glUniform2f(glGetUniformLocation(m_MorphProgram, "quad_scale"), 
                    rnd_s / dst_w, rnd_s / dst_h );
        
        // Set Active Texture #0
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(m_MorphProgram, "inputImage"), 0);
        
        // Set Beier & Neely Parameters
        glUniform3f(glGetUniformLocation(m_MorphProgram, "algoParams"), 
                    m_P, m_A / rnd_s, m_B);
        
        // Set Line Count
        glUniform1i(glGetUniformLocation(m_MorphProgram, "linesCount"), 
                    line_num);
        
        // Set Line PQ parameters
        //glUniform4fv(glGetUniformLocation(m_MorphProgram, "linesPQ"), 
        //             line_num, m_LinesX.data());
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_LinesXUBO);
        
        // Disable Blending
        glDisable(GL_BLEND);
        
        // Render Image #0
        glBindTexture(GL_TEXTURE_2D, m_Texture0);
        glUniform2f(texcoordScaleLoc, rnd_s/img0_w, rnd_s/img0_h);
        //glUniform4fv(linesPQbarLoc, line_num, m_Lines0.data());
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_Lines0UBO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        // Enable Blending
        glEnable(GL_BLEND);
        
        // Render Image #1
        glBindTexture(GL_TEXTURE_2D, m_Texture1);
        glUniform2f(texcoordScaleLoc, rnd_s/img1_w, rnd_s/img1_h);
        //glUniform4fv(linesPQbarLoc, line_num, m_Lines1.data());
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_Lines1UBO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        // Reset Current Vertex Array Object
        glBindVertexArray(0);
        
        // Reset Current Program
        glUseProgram(0);
        
        // Disable Blending
        glDisable(GL_BLEND);
        
        // Reset Current Framebuffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      }
    }
    resetDirty();
  }
  
}
