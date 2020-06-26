#version 400
uniform sampler2D t;

varying FragData{
    highp vec4 fragPosition;
    highp vec4 fragNormal;
    highp vec2 fragCoord;
    flat highp vec4 fragMeta;
} fragData;
uniform highp vec4 posEye;
uniform highp vec4 posLight;
uniform highp vec4 colLight;
uniform highp float emissive;
uniform highp vec4 diffuse;
uniform highp float specular;
//uniform float shininess;
uniform highp vec4 ambient;
uniform highp float brightness;
uniform highp vec3 tone;
layout(location=0) out highp vec4 vFragColor;
layout(location=1) out highp vec4 vFragMeta;

void main(void)
{
    /*
    //Only considering emissive, diffuse and ambient.
    highp vec4 l1=emissive;
    highp vec4 normal=normalize(fragData.fragNormal);
    highp vec4 light_ray=normalize(posLight-fragData.fragPosition);
    vec4 eyesight=normalize(posEye-fragPosition);
    vec4 aver=normalize(light_ray+eyesight);
    highp vec4 reflection=reflect(-light_ray,normal);
    highp vec4 l2=max(dot(normal,light_ray),0.0f)*colLight*diffuse;
    //vec4 l3=pow(max(dot(reflection,eyesight),0)*colLight*specular);
    highp vec4 total_light=l1+l2+ambient; //ignoring specular
    vFragColor=brightness*total_light*texture(t,fragData.fragCoord.st);
    vFragMeta=fragData.fragMeta;
    */

    highp vec4 normal=normalize(fragData.fragNormal);
    highp vec4 light_ray=normalize(posLight-fragData.fragPosition);
    highp float dflight=max(dot(normal,light_ray),0.0f)*0.8f;
    highp vec4 eyesight=normalize(posEye-fragData.fragPosition);
    highp vec4 reflection=reflect(-light_ray,normal);
    highp vec4 aver=normalize(light_ray+eyesight);
    highp float spelight=pow(max(dot(aver,normal),0),specular*2)*dflight;
    highp float total_light=dflight+emissive;
    vFragColor=vec4(tone,1)*(total_light*texture(t,fragData.fragCoord.st));
    vFragMeta=vec4(fragData.fragMeta.rgb,1.f);
}
