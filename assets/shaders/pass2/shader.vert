#version 420
in highp vec4 vertAttr;
in highp vec4 texCoord;
//in highp vec4 vertTone;
uniform highp mat4 matrix;

out highp vec4 coord;
//out highp vec4 fragTone;
void main(void)
{
    gl_Position = matrix * vertAttr;
    coord=texCoord;
    //fragTone=vertTone;
}
