#version 320 es

layout (location = 0) in vec3 position;

const vec3 CAMERA_POS = vec3(0., 3., -9);

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
}