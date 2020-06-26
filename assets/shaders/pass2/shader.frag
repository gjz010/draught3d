#version 420
uniform sampler2DMS t;
uniform sampler2DMS meta;
uniform highp float brightness;
uniform bool renderTone;
in highp vec4 coord;
in highp vec4 fragTone;
layout(location=0) out highp vec4 vFragColor;
layout(location=1) out highp vec4 clickMeta;
vec4 textureMultisample(sampler2DMS sampler, ivec2 ipos)
{
        vec4 color = vec4(0.0,0.0,0.0,0.0);

        for (int i = 0; i < 4; i++)
        {
                color += texelFetch(sampler, ipos, i);
        }

        color /= float(4);

        return color;
}
void main(void)
{
    ivec2 size=textureSize(t);
    ivec2 gc=ivec2(vec2(1600,1200)*coord.st);
    highp vec4 color=textureMultisample(t,gc);
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
    highp vec4 red=vec4(1.f,0.f,0.f,1.f);
    highp vec4 green=vec4(0.f,1.f,0.f,1.f);
    vFragColor=color;
    clickMeta=texelFetch(meta,gc,0);
    //vFragColor=red;
    //clickMeta=green;
    //
}
