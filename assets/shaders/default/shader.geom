#version 400
layout(triangles) in;
layout(triangle_strip,max_vertices=3) out;
uniform highp float explode_distance;
varying GeomData{
    highp vec4 geomPosition;
    highp vec4 geomNormal;
    highp vec2 texCoord;
    flat highp vec4 geomMeta;
} geomData[];

out FragData{
    highp vec4 fragPosition;
    highp vec4 fragNormal;
    highp vec2 fragCoord;
    flat highp vec4 fragMeta;
} fragData;


uniform float explosion_time;
vec3 getTriangleNormal(){
    vec3 a=vec3(gl_in[0].gl_Position)-vec3(gl_in[1].gl_Position);
    vec3 b=vec3(gl_in[2].gl_Position)-vec3(gl_in[1].gl_Position);
    return normalize(cross(a,b));
    //return vec3(0.f,1.f,0.f);
}
vec4 explode(vec4 position,vec3 normal){
    float magnitude=3.0f;
    vec3 dir=normal*explode_distance;
    return position+vec4(dir,0);
}
void main(void)
{
    vec3 normal=getTriangleNormal();
    for(int i=0;i<3;i++){
        gl_Position=explode(gl_in[i].gl_Position,normal);
        fragData.fragPosition=explode(geomData[i].geomPosition,normal);
        fragData.fragNormal=geomData[i].geomNormal;
        fragData.fragCoord=geomData[i].texCoord;
        fragData.fragMeta=geomData[i].geomMeta;
        EmitVertex();
    }

    //vec3 normal=getTriangleNormal();
    EndPrimitive();

}
