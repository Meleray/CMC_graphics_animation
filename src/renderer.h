#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <chrono>

class Renderer
{
public:
    void Init(SDL_Window *window, int w, int h);
    void Render();
    void Close();
private:
    GLuint VAO, VBO, EBO, colorbuffer, typebuffer, texbuffer, texture;
    GLuint shaderProgram;
    SDL_Window *window;
    int width,height;
    std::chrono::high_resolution_clock::time_point time_beg;
};