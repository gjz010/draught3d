#include "widget.h"
#include "QDebug"
#include <QtOpenGL/QGL>
#include "QOpenGLShaderProgram"
#include "vertex.h"
#include "QTimer"
#include "QOpenGLFramebufferObject"
#include "QMatrix4x4"
#include "QProcessEnvironment"
#include "QtGlobal"
#include "QApplication"
#include "QDesktopWidget"
#include "QPushButton"
#include "draughtclient.h"
#include "draughtserver.h"
#include "scene.h"
#include "scenemenu.h"
#include "simpleinputmanager.h"
struct Widget::Internal{
    ModelData chess;
    //QOpenGLFramebufferObject* fbo;
    GLuint raw_framebuffer;
    GLuint raw_colorbuffer;
    GLuint raw_clickbuffer;
    GLuint raw_depthstencilbuffer;
    GLuint clickframebuffer;
    GLuint click_mapping_buffer;
    GLuint color_buffer;
    int width;
    int height;

    DraughtClient* current;
    DraughtServer* local;
    Scene* scene=nullptr;
    SimpleInputManager* input;
};
Widget::Widget(QOpenGLWidget *parent) :
    QOpenGLWidget(parent)
{
    QSurfaceFormat format=this->format();
    format.setSamples(1);
    this->setFormat(format);
    gl=new GLManager();
    data=new Internal;
    int scale=QApplication::desktop()->devicePixelRatio();
    data->width= scale*this->width();
    data->height= scale*this->height();
    /*
    QPushButton* b=new QPushButton("Click!",this);
    b->setGeometry(10,10,100,100);
    */
    data->input=new SimpleInputManager(this);
    input()->setViewport(this->width(),this->height());
    this->setMouseTracking(true);
}

Widget::~Widget()
{
    delete data;
    //glDeleteFramebuffers(1,&data->raw_framebuffer);
}

void Widget::initializeGL()
{

    makeCurrent();
    initializeOpenGLFunctions();
    //glEnable(GL_MULTISAMPLE);
    //glEnable(GL_CULL_FACE);
    //glClearDepth(2000.0);

    this->glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
    this->glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    gl->LoadModel("chess");
    gl->LoadModel("chessboard");
    gl->LoadModel("test");
    Model* model=gl->getModel("chess");
    data->chess=model->writeToBuffer(nullptr,nullptr);


    //qDebug()<<QString("%1 %2").arg(data.idx_count).arg(data.vert_count);
    gl->LoadShader("default");
    gl->LoadShader("test");
    gl->LoadShader("pass2");
    //this->data->buf=buffer;
    gl->LoadTexture("gold_chess");
    gl->LoadTexture("silver_chess");
    gl->LoadTexture("chessboard");
    //gl->LoadTexture("chessboard_4096");

    /*
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(1);
    data->fbo=new QOpenGLFramebufferObject(1600,1200,format);
    data->fbo->
    */

    //Raw operations below.
    glGenFramebuffers(1,&data->raw_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,data->raw_framebuffer);
    GLuint texColorBuffer;
    glGenTextures(1,&texColorBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,texColorBuffer);
    //glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,00,1200,0,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,4,GL_RGB,1600,1200,true);
    qDebug()<<this->glGetError();
    //glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //qDebug()<<this->glGetError();
    //glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //qDebug()<<this->glGetError();
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,0);
    qDebug()<<this->glGetError();
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D_MULTISAMPLE,texColorBuffer,0);
    qDebug()<<this->glGetError();
    GLuint texClickBuffer;
    glGenTextures(1,&texClickBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,texClickBuffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,4,GL_RGB,1600,1200,true);
    //glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,1600,1200,0,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D_MULTISAMPLE,texClickBuffer,0);
    GLuint rbo;
    glGenRenderbuffers(1,&rbo);
    glBindRenderbuffer(GL_RENDERBUFFER,rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER,4,GL_DEPTH24_STENCIL8,1600,1200);
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,rbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    qDebug()<<glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER,0);


    glGenFramebuffers(1,&data->clickframebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,data->clickframebuffer);

    glGenTextures(1,&data->color_buffer);
    glBindTexture(GL_TEXTURE_2D,data->color_buffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,1600,1200,0,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,data->color_buffer,0);

    glGenTextures(1,&data->click_mapping_buffer);
    glBindTexture(GL_TEXTURE_2D,data->click_mapping_buffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,1600,1200,0,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,data->click_mapping_buffer,0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    /*
    glBindFramebuffer(GL_FRAMEBUFFER,this->defaultFramebufferObject());
    glGenTextures(1,&data->click_mapping_buffer);
    glBindTexture(GL_TEXTURE_2D,data->click_mapping_buffer);
    glBindTexture(GL_TEXTURE_2D,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,data->click_mapping_buffer,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    */

    data->raw_colorbuffer=texColorBuffer;
    data->raw_clickbuffer=texClickBuffer;
    data->raw_depthstencilbuffer=rbo;



    //Raw operations above.


    data->scene=new SceneMenu(gl,this);

    //Launch!
    QTimer* timer=new QTimer(this);
    timer->setInterval(16);

    connect(timer,SIGNAL(timeout()),this,SLOT(doFrame()));
    timer->start();

}

void Widget::resizeGL(int width, int height)
{
    //qDebug()<<width<<height;
    int scale=QApplication::desktop()->devicePixelRatio();
    data->width= scale*this->width();
    data->height= scale*this->height();
    input()->setViewport(this->width(),this->height());

}
float angle=0;
void Widget::paintGL()
{

    glViewport(0,0,1600,1200);
    GLenum opts[]={GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
    makeCurrent();
    glBindFramebuffer(GL_FRAMEBUFFER, data->raw_framebuffer);
    glDrawBuffers(2,opts);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    if(data->scene!=nullptr) data->scene->render();
    //glFinish();
    glDisable(GL_MULTISAMPLE);
    //Pass2: Draw the color buffer to single buffer.
    GLuint pass2buf=data->clickframebuffer;
    //pass2buf=this->defaultFramebufferObject();
    //qDebug()<<pass2buf;
    glBindFramebuffer(GL_FRAMEBUFFER,pass2buf);
    //glBindFramebuffer(GL_FRAMEBUFFER,data->clickframebuffer);
    //glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffers(2,opts);
    glDisable(GL_DEPTH_TEST);
    this->glClearColor(0.f,0.f,0.f,1.f);
    this->glClear(GL_COLOR_BUFFER_BIT);


    //makeCurrent();
    QOpenGLShaderProgram* secpass_shader=gl->getShader("pass2");
    Vertex quad[]={
      {{0.f,1.f,0.f},{0.f,0.f,0.f},{0.f,0.f}},
      {{0.f,0.f,0.f},{0.f,0.f,0.f},{0.f,1.f}},
      {{1.f,0.f,0.f},{0.f,0.f,0.f},{1.f,1.f}},
      {{1.f,1.f,0.f},{0.f,0.f,0.f},{1.f,0.f}},

    };
    float* arr=(float*)&quad;
    secpass_shader->bind();

    QMatrix4x4 tmpmat;
    tmpmat.ortho(0,1,0,1,-1,1);
    secpass_shader->setUniformValue("matrix",tmpmat);
    secpass_shader->enableAttributeArray("vertAttr");
    secpass_shader->enableAttributeArray("texCoord");
    secpass_shader->setAttributeArray("vertAttr",GL_FLOAT,arr,3,sizeof(Vertex));
    secpass_shader->setAttributeArray("texCoord",GL_FLOAT,arr+6,2,sizeof(Vertex));
    //gl->getTexture("gold_chess")->bind();
    secpass_shader->setUniformValue("t",0);
    secpass_shader->setUniformValue("meta",1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,data->raw_colorbuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,data->raw_clickbuffer);

    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,0);
    secpass_shader->release();




    //Pass3:Blit
    glViewport(0,0,data->width,data->height);
    glBindFramebuffer(GL_FRAMEBUFFER,this->defaultFramebufferObject());
    this->glClearColor(1.f,0.f,0.f,1.f);
    this->glClear(GL_COLOR_BUFFER_BIT);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    //makeCurrent();
    QOpenGLShaderProgram* thrdpass_shader=gl->getShader("test");
    thrdpass_shader->bind();

    thrdpass_shader->setUniformValue("matrix",tmpmat);
    thrdpass_shader->enableAttributeArray("vertAttr");
    thrdpass_shader->enableAttributeArray("texCoord");
    thrdpass_shader->setAttributeArray("vertAttr",GL_FLOAT,arr,3,sizeof(Vertex));
    thrdpass_shader->setAttributeArray("texCoord",GL_FLOAT,arr+6,2,sizeof(Vertex));
    //gl->getTexture("gold_chess")->bind();
    thrdpass_shader->setUniformValue("t",0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,data->color_buffer);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glBindTexture(GL_TEXTURE_2D,0);
    thrdpass_shader->release();




}

QVector3D Widget::getClickedMeta(int x,int y)
{
    int px=(double)x/this->width()*1600;
    int py=(double)y/this->height()*1200;
    QVector4D vec;
    GLuint tmp;
    glGenFramebuffers(1,&tmp);
    //glBindFramebuffer(GL_FRAMEBUFFER,0);
    glBindFramebuffer(GL_FRAMEBUFFER,tmp);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,data->click_mapping_buffer,0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(px,py,1,1,GL_RGBA,GL_FLOAT,&vec);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    //qDebug()<<color[0]<<color[1]<<color[2]<<color[3]<<glGetError();
    glDeleteFramebuffers(1,&tmp);
    return vec.toVector3D();
}

void Widget::mousePressEvent(QMouseEvent *ev)
{
    input()->onMouseDown(ev->x(),ev->y(),getClickedMeta(ev->x(),ev->y()));
}

void Widget::mouseMoveEvent(QMouseEvent *ev)
{
    input()->onMouseMove(ev->x(),ev->y(),getClickedMeta(ev->x(),ev->y()));
}

void Widget::mouseReleaseEvent(QMouseEvent *ev)
{
    input()->onMouseUp(ev->x(),ev->y(),getClickedMeta(ev->x(),ev->y()));
}

void Widget::wheelEvent(QWheelEvent *ev)
{
    input()->onMouseWheel(ev->delta());
}

void Widget::keyPressEvent(QKeyEvent *ev)
{
    input()->translateKey(ev->key(),true);
}

void Widget::keyReleaseEvent(QKeyEvent *ev)
{
    input()->translateKey(ev->key(),false);
}


void Widget::paintEvent(QPaintEvent * ev)
{
    QOpenGLWidget::paintEvent(ev);
    //QPainter painter(this);
    //painter.drawImage(0,0,QImage(":/assets/images/chessboard.png"));
}

void Widget::renderModel(ModelData md, QMatrix4x4 modelMat, QVector4D meta,QString texture,QOpenGLShaderProgram* shader,Material mat)
{
    for(int i=0;i<4;i++){
        shader->setAttributeValue(3+i,modelMat.column(i));

    }
    shader->setUniformValue("useModelMat",true);
    shader->setAttributeValue(7,meta);
    md.vertex_buffer->bind();
    shader->setAttributeBuffer("vertAttr",GL_FLOAT,offsetof(Vertex,vertAttr),3,sizeof(Vertex));
    shader->setAttributeBuffer("vertNormal",GL_FLOAT,offsetof(Vertex,vertNormal),3,sizeof(Vertex));
    shader->setAttributeBuffer("texCoord",GL_FLOAT,offsetof(Vertex,texCoord),2,sizeof(Vertex));
    shader->setUniformValue("emissive",mat.emissive);
    shader->setUniformValue("specular",mat.specular);
    md.vertex_buffer->release();
    shader->enableAttributeArray("vertAttr");
    shader->enableAttributeArray("vertNormal");
    shader->enableAttributeArray("texCoord");
    gl->getTexture(texture)->bind();
    //qDebug()<<data->chess.index_buffer->bind()<<data->chess.index_buffer->type();
    //qDebug()<<data->chess.idx_count;
    //data->chess.vertex_buffer->write(0,vertices,sizeof(Vertex)*3);
    //this->glDrawArrays(GL_TRIANGLES,0,data->chess.idx_count);
    //int indices[]={0,1,2};
    md.index_buffer->bind();
    this->glDrawElements(GL_TRIANGLES,md.idx_count,GL_UNSIGNED_INT,0);
    //qDebug("Error?");
    //qDebug()<<this->glGetError();
    //data->chess.vertex_buffer->release();
    md.index_buffer->release();


}

SimpleInputManager *Widget::input()
{
    return data->input;
}

void Widget::doFrame()
{
    if(data->scene!=nullptr) data->scene->act();
    update();
}
