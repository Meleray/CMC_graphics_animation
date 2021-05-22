#version 320 es

precision mediump float;

in vec3 fragmentColor;
in vec2 UV;
out vec4 fragColor;

uniform sampler2D text;
uniform vec3 color;

void main()
{
    if (fragmentColor.x == -1.0) {
        if (texture(text, UV).a < 1.0) {
            discard;
        }
        fragColor = vec4(texture(text, UV).rgb, 1);
    } else {
        fragColor = vec4(fragmentColor,1);
    }
}