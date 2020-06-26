#include "model.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <unordered_map>
#include <map>
Model::Model(const QString& obj,QObject* parent):obj(obj),QObject(parent)
{

}

void Model::Load()
{
    buffer_size=0;
    vertAttr.reserve(1024);
    vertNormal.reserve(1024);
    texCoord.reserve(1024);
    /*
    vertIndices.reserve(2048);
    texIndices.reserve(2048);
    normalIndices.reserve(2048);
    */
    indices.reserve(8000);
    QFile f(obj);
    f.open(QFile::ReadOnly);
    QTextStream stream(&f);
    int faces=0;
    while(!stream.atEnd()){
        //qDebug("Line!");
        QString str=stream.readLine();
        if(str=="") continue;
        if(str.at(0).toLatin1()=='#') continue;
        QStringList list=str.split(" ",QString::SkipEmptyParts);
        if(list.size()<2) continue;
        QString op=list.at(0).toLower();
        if(op=="mtllib") continue;
        if(op=="o") continue;
        if(op=="v"){

            float x=list.at(1).toFloat();
            float y=list.at(2).toFloat();
            float z=list.at(3).toFloat();
            vertAttr.push_back(QVector3D(x,y,z));
            continue;
        }
        if(op=="vt"){
            float u=list.at(1).toFloat();
            float v=1.f-list.at(2).toFloat();
            texCoord.push_back(QVector2D(u,v));
            continue;
        }
        if(op=="vn"){
            float x=list.at(1).toFloat();
            float y=list.at(2).toFloat();
            float z=list.at(3).toFloat();
            vertNormal.push_back(QVector3D(x,y,z));
            continue;
        }
        if(op=="usemtl") continue;
        if(op=="s") continue;
        if(op=="f"){
            for(int i=0;i<3;i++){
                QStringList tuple=list.at(i+1).split("/",QString::SkipEmptyParts);
                int idxVert=tuple.at(0).toInt()-1;
                int idxTex=tuple.at(1).toInt()-1;
                int idxNormal=tuple.at(2).toInt()-1;
                IndexTriple triple={idxVert,idxNormal,idxTex};
                indices.push_back(triple);
                buffer_size++;

            }
            faces++;
            continue;
        }



    }
    qDebug()<<QString("Model with %1 vertices, %2 normals, %3 texcoords and %4 faces loaded.").arg(vertAttr.size()).arg(vertNormal.size()).arg(texCoord.size()).arg(faces);
}

bool Model::isLoaded() const
{
    return loaded;
}

Model::~Model()
{

}

ModelData Model::writeToBuffer(QOpenGLBuffer *vertex_buffer,QOpenGLBuffer* index_buffer,bool compress)
{
    std::vector<Vertex> vertData;
    std::vector<int> idxData;
    vertData.reserve(buffer_size);
    idxData.reserve(buffer_size);
    int idx=0;
    if(compress){

        std::map<IndexTriple,int> vert_set;

        for(int i=0;i<buffer_size;i++){

            IndexTriple triple=indices[i];
            if(vert_set.find(triple)==vert_set.end()){
                Vertex vert;
                vert.vertAttr=vertAttr.at(triple.vertIndex);
                vert.vertNormal=vertNormal.at(triple.normalIndex);
                vert.texCoord=texCoord.at(triple.texIndex);
                //qDebug()<<vert.vertAttr<<vert.vertNormal<<vert.texCoord;
                //qDebug()<<idx;
                vertData.push_back(vert);
                idxData.push_back(idx);
                vert_set.insert(std::make_pair(triple,idx));
                idx++;
            }else{
                //qDebug("Same vert!");
                int tmp=vert_set.at(triple);
                idxData.push_back(tmp);
                //qDebug()<<tmp;

            }


        }

    }else{
        for(int i=0;i<buffer_size;i++){
            IndexTriple triple=indices[i];
            Vertex vert;
            vert.vertAttr=vertAttr.at(triple.vertIndex);
            vert.vertNormal=vertNormal.at(triple.normalIndex);
            vert.texCoord=texCoord.at(triple.texIndex);
            vertData.push_back(vert);
            idxData.push_back(idx);
            idx++;
        }

    }

    if(vertex_buffer==nullptr) vertex_buffer=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vertex_buffer->create();
    vertex_buffer->bind();
    vertex_buffer->allocate(sizeof(Vertex)*vertData.size());

    qDebug()<<vertData.size();
    vertex_buffer->write(0,vertData.data(),sizeof(Vertex)*vertData.size());
    vertex_buffer->release();

    if(index_buffer==nullptr) index_buffer=new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    index_buffer->create();
    index_buffer->bind();
    index_buffer->allocate(sizeof(int)*idxData.size());

    qDebug()<<idxData.size();
    index_buffer->write(0,idxData.data(),sizeof(int)*idxData.size());
    index_buffer->release();

    //qDebug()<<*f;
    //empty_buffer->release();
    ModelData data={vertex_buffer,index_buffer,vertData.size(),idxData.size()};
    return data;

}

size_t Model::IndexTriple::Hash::operator()(const Model::IndexTriple &vert) const
{
    std::string str=QString("%1/%2/%3").arg(vert.vertIndex).arg(vert.normalIndex).arg(vert.texIndex).toStdString();
    //qDebug(str.c_str());
    std::hash<std::string> hash;
    return hash(str);
}

bool Model::IndexTriple::operator<(const Model::IndexTriple &rh) const
{
    if(this->vertIndex<rh.vertIndex) return true;
    if(this->vertIndex>rh.vertIndex) return false;
    if(this->texIndex<rh.texIndex) return true;
    if(this->texIndex>rh.texIndex) return false;
    if(this->normalIndex<rh.normalIndex) return true;
    if(this->normalIndex>rh.normalIndex) return false;
    return false;
}

bool Model::IndexTriple::operator==(const Model::IndexTriple &rh) const
{
    return vertIndex==rh.vertIndex && rh.texIndex==texIndex && rh.normalIndex==normalIndex;
}
