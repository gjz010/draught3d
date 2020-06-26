#ifndef VERTEX_H
#define VERTEX_H
#include "QVector3D"
#include "QVector2D"
#include "QString"
class Vertex
{
public:
    QVector3D vertAttr;
    QVector3D vertNormal;
    QVector2D texCoord;
    struct Hash{
        size_t operator()(const Vertex& vert) const;

    };
    bool operator==(const Vertex& rh) const;
};

#endif // VERTEX_H
