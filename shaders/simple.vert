#version 320 es

layout (location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in int type;
layout(location = 3) in vec2 uv;

out vec3 fragmentColor;
out vec2 UV;

uniform mat4 MVP;
uniform float time;

void main()
{
    if (type == 0) {
        gl_Position =  MVP * vec4(position.x, position.y, position.z,1);
        fragmentColor = color;
    } else {
        gl_Position =  MVP * vec4(position.x, position.y, position.z + position.x * cos(position.x + time),1);
        fragmentColor = vec3(-1, -1, -1);
    }
    UV = uv;
}