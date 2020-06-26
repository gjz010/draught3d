#include "vertex.h"
#include "QString"

bool Vertex::operator==(const Vertex &rh) const
{
    return vertAttr==rh.vertAttr && vertNormal==rh.vertNormal && texCoord==rh.texCoord;
}

size_t Vertex::Hash::operator()(const Vertex &vert) const
{
    QString str1=QString("{%1,%2,%3}").arg(vert.vertAttr.x()).arg(vert.vertAttr.y()).arg(vert.vertAttr.z());
    QString str2=QString("{%1,%2,%3}").arg(vert.vertNormal.x()).arg(vert.vertNormal.y()).arg(vert.vertNormal.z());
    QString str3=QString("{%1,%2}").arg(vert.texCoord.x()).arg(vert.texCoord.y());
    QString str=QString("{%1,%2,%3}").arg(str1).arg(str2).arg(str3);
    std::hash<std::string> obj;
    return obj(str.toStdString());

}
