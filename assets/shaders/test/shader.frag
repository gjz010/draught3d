#version 420
uniform sampler2D t;
uniform highp float brightness;
in highp vec4 coord;
out highp vec4 vFragColor;
void main(void)
{
    highp vec4 color=texture(t,coord.st);
    /*
    if(renderTone){
        if(fragTone.a>1.0f){
            highp float greyscale=dot(fragTone.rgb,color.rgb);
            color=vec4(greyscale,greyscale,greyscale,color.a*(fragTone.a-1.f));

        }else{
            color=color*fragTone;

        }

    }
    color=min(color,1.f);
    */
    vFragColor=color;
    //vFragColor=vec4(1.f,1.f,1.f,1.f);
}
