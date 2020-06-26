#ifndef GLMANAGER_H
#define GLMANAGER_H

#include <QObject>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include <QtOpenGL/QGL>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QSound>

#include "model.h"
using std::string;
using std::map;
using std::stack;
using std::vector;
class GLManager : public QObject
{
    Q_OBJECT
    std::map<QString,QOpenGLShaderProgram*> shaders;
    std::map<QString,int> shader_attr;
    std::vector<QOpenGLTexture*> textures;
    std::map<QString,int> texture_mapping;
    std::map<QString,QSound*> sound_mapping;
    std::map<QString,ModelData> baked_model;
    std::stack<QMatrix4x4,std::vector<QMatrix4x4> > matrix_model;
    std::stack<QMatrix4x4,std::vector<QMatrix4x4> > matrix_projection;
    std::stack<QMatrix4x4,std::vector<QMatrix4x4> > matrix_view;
    std::map<QString,Model*> model_mapping;
public:
    explicit GLManager(QObject *parent = nullptr);
    QOpenGLShaderProgram* LoadShader(const QString& name);
    int LoadShaderAttribute(const QString& shader_name,const QString& attr,const QOpenGLShaderProgram* shader=nullptr);
    QOpenGLShaderProgram* getShader(const QString& name);
    void popModelMatrix();
    void popProjMatrix();
    void popViewMatrix();
    void pushModelMatrix();
    void pushProjMatrix();
    void pushViewMatrix();
    QMatrix4x4& getModelMatrix() const;
    QMatrix4x4& getProjMatrix() const;
    QMatrix4x4& getViewMatrix() const
    {
        QMatrix4x4& mat=(QMatrix4x4&)matrix_view.top();
        return mat;
    }
    QMatrix4x4 getMatrix() const;
    void initStack();
    int LoadTexture(const QString& name);
    QOpenGLTexture* getTexture(const QString& name);
    QOpenGLTexture* getTexture(int idx) const;
    QVector3D unproject(float x,float y,float z);
    //Indeed it's not something that GL manager should do...
    void LoadSound(const QString& name);
    QSound* getSound(const QString& name) const;
    void LoadModel(const QString& name);
    Model* getModel(const QString& name);
    void bakeModel(const QString& name);
    ModelData getBakedModel(const QString& name);

signals:

public slots:
};

#endif // GLMANAGER_H
