#include "glmanager.h"
#include <QFile>
#include <QTextStream>
#include <utility>

GLManager::GLManager(QObject *parent) : QObject(parent)
{
    initStack();
}

QOpenGLShaderProgram* GLManager::LoadShader(const QString &name)
{
    QOpenGLShaderProgram* prog=new QOpenGLShaderProgram(this);
    prog->addShaderFromSourceFile(QOpenGLShader::Vertex,":/assets/shaders/"+name+"/shader.vert");
    prog->addShaderFromSourceFile(QOpenGLShader::Fragment,":/assets/shaders/"+name+"/shader.frag");
    prog->addShaderFromSourceFile(QOpenGLShader::Geometry,":/assets/shaders/"+name+"/shader.geom");
    prog->link();
    shaders.insert(std::make_pair(name,prog));
    return prog;
}

int GLManager::LoadShaderAttribute(const QString &shader_name, const QString &attr, const QOpenGLShaderProgram *shader)
{
    if(shader==nullptr) shader=getShader(shader_name);
    int arg=shader->attributeLocation(attr);
    shader_attr.insert(std::make_pair(shader_name+"|"+attr,arg));
    return arg;
}

QOpenGLShaderProgram *GLManager::getShader(const QString &name)
{
    return shaders.at(name);
}

void GLManager::popViewMatrix()
{
    matrix_view.pop();
}

void GLManager::popProjMatrix()
{
    matrix_projection.pop();
}

void GLManager::popModelMatrix()
{
    matrix_model.pop();
}

void GLManager::pushViewMatrix()
{
    matrix_view.push(matrix_view.top());
}

void GLManager::pushProjMatrix()
{
    matrix_projection.push(matrix_projection.top());
}
void GLManager::pushModelMatrix()
{
    matrix_model.push(matrix_model.top());
}



QMatrix4x4 &GLManager::getModelMatrix() const
{
    QMatrix4x4& mat=(QMatrix4x4&)matrix_model.top();
    return mat;
}

QMatrix4x4 &GLManager::getProjMatrix() const
{
    return (QMatrix4x4&)matrix_projection.top();
}

QMatrix4x4 GLManager::getMatrix() const
{
    return getProjMatrix()*getViewMatrix()*getModelMatrix();
}

void GLManager::initStack()
{
    matrix_view=std::move(std::stack<QMatrix4x4,std::vector<QMatrix4x4> >());
    matrix_projection=std::move(std::stack<QMatrix4x4,std::vector<QMatrix4x4> >());
    matrix_model=std::move(std::stack<QMatrix4x4,std::vector<QMatrix4x4> >());
    matrix_view.push(QMatrix4x4());
    matrix_projection.push(QMatrix4x4());
    matrix_model.push(QMatrix4x4());
}

QOpenGLTexture *GLManager::getTexture(int idx) const
{
    return textures[idx];
}

QVector3D GLManager::unproject(float x, float y, float z)
{
    QVector3D vec(x,1.f-y,z);
    return vec.unproject(getViewMatrix(),getProjMatrix(),QRect(0,0,1,1));
}

void GLManager::LoadSound(const QString &name)
{
    qDebug()<<name;
    sound_mapping.insert(std::make_pair(name,new QSound(":/assets/sounds/"+name)));
}

QSound *GLManager::getSound(const QString &name) const
{
    return sound_mapping.at(name);
}

void GLManager::LoadModel(const QString &name)
{
    Model* model=new Model(":/assets/models/"+name+".obj",this);
    model->Load();
    model_mapping.insert(std::make_pair(name,model));
}

Model *GLManager::getModel(const QString &name)
{
    return model_mapping.at(name);
}

void GLManager::bakeModel(const QString &name)
{
    ModelData data=getModel(name)->writeToBuffer(nullptr,nullptr);
    baked_model.insert(std::make_pair(name,data));
}

ModelData GLManager::getBakedModel(const QString &name)
{
    return baked_model.at(name);
}

int GLManager::LoadTexture(const QString &name)
{
    int idx=textures.size();
    QOpenGLTexture* texture=new QOpenGLTexture(QImage(QString(":/assets/images/%1.png").arg(name)));
    textures.push_back(texture);
    texture_mapping.insert(std::make_pair(name,idx));
    texture->release();
    return idx;
}

QOpenGLTexture *GLManager::getTexture(const QString &name)
{   int idx;
    if(texture_mapping.find(name)==texture_mapping.end()){
        idx=LoadTexture(name);
        //return getTexture(); //Not recommended, may cause lag!

    }else{
        idx=texture_mapping.at(name);

    }
    //qDebug()<<idx;
    return getTexture(idx);
}

