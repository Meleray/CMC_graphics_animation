#version 320 es

precision mediump float;

in vec3 fragmentColor;
int vec2 UV;
out vec4 fragColor;

uniform sampler2D text;
uniform vec3 color;

void main()
{
    if (fragColor.x != -1) 
        fragColor = vec4(fragmentColor,1);
    else {
        fragColor = texture(text, UV).rgb;
    }
}