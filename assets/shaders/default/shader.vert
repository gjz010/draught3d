#version 400
layout (location=0) in highp vec3 vertAttr;
layout (location=1) in highp vec3 vertNormal;
layout (location=2) in highp vec2 texCoord;
layout (location=3) in highp vec4 matc1;
layout (location=4) in highp vec4 matc2;
layout (location=5) in highp vec4 matc3;
layout (location=6) in highp vec4 matc4;
layout (location=7) in highp vec4 click_meta;
uniform bool useModelMat;

uniform highp mat4 matrix;
varying GeomData{
    highp vec4 geomPosition;
    highp vec4 geomNormal;
    highp vec2 texCoord;
    flat highp vec4 geomMeta;
} geomData;

void main(void)
{
    if(useModelMat){
        mat4 model=mat4(matc1,matc2,matc3,matc4);
        gl_Position = matrix*model*vec4(vertAttr,1);
        geomData.geomNormal=matrix*model*vec4(vertNormal,0);
    }else{
        gl_Position = matrix*vec4(vertAttr,1);
        geomData.geomNormal=matrix*vec4(vertNormal,0);

    }

    geomData.geomPosition=gl_Position;

    geomData.texCoord=texCoord;
    geomData.geomMeta=click_meta;
}
