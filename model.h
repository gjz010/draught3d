#ifndef MODEL_H
#define MODEL_H
#include "QVector4D"
#include "QVector3D"
#include "QVector2D"
#include "vector"
#include <QObject>
#include "QOpenGLBuffer"
#include "vertex.h"
struct ModelData{
    QOpenGLBuffer* vertex_buffer;
    QOpenGLBuffer* index_buffer;
    int vert_count;
    int idx_count;
};
struct Material{
    float specular=0.f;
    float emissive=0.f;
};
class Model : public QObject
{
    Q_OBJECT
public:
    Model(const QString& obj,QObject* parent);
    Model(const Model& copy)=delete;
    Model(Model&& copy)=delete;
    Model& operator=(const Model& copy)=delete;
    Model& operator=(Model&& move)=delete;
    void Load();
    bool isLoaded() const;
    ~Model();
    int calculateSize();
    ModelData writeToBuffer(QOpenGLBuffer *vertex_buffer=nullptr,QOpenGLBuffer* index_buffer=nullptr,bool compress=true);
    int buffer_size;
private:
    struct IndexTriple{
        int vertIndex;
        int normalIndex;
        int texIndex;
        struct Hash{
            size_t operator()(const IndexTriple& vert) const;

        };
        bool operator<(const IndexTriple& rh) const;
        bool operator==(const IndexTriple& rh) const;
    };
    bool loaded=false;
    QString obj;
    //QString mtl;

    std::vector<QVector3D> vertAttr;
    std::vector<QVector3D> vertNormal;
    std::vector<QVector2D> texCoord;
    std::vector<IndexTriple> indices;
    /*
    std::vector<int> vertIndices;
    std::vector<int> texIndices;
    std::vector<int> normalIndices;
    */
};

#endif // MODEL_H
