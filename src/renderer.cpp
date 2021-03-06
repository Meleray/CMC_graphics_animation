#include "renderer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

const float FLAG_LENGTH = 0.8;
const int FLAGH = 15;
const int FLAGW = 24;

    int add_shader(std::string fileName, GLuint shaderProgram, GLenum shaderType)
    {
        char* src; int32_t size;

        //Читаем файл с кодом шейдера
        std::ifstream t;
        std::string fileContent;

        t.open(fileName);
        if(t.is_open())
        {
            std::stringstream buffer;
            buffer << t.rdbuf();
            fileContent = buffer.str();
        }
        else std::cout<<"File "<<fileName<<" opening failed"<<std::endl;
        t.close();
        size = fileContent.length();  //Set the Size

        std::string result = fileContent;
        src = const_cast<char*>(result.c_str());

        int shaderID = glCreateShader(shaderType);
        glShaderSource(shaderID, 1, &src, &size);
        
        //компилируем шейдер
        glCompileShader(shaderID);
        int success;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if(success) 
        {
            glAttachShader(shaderProgram, shaderID);
        }
        else 
        {
            //печатаем информацию об ошибках компиляции
            int m;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &m);
            char* l = new char[m];
            if(t) glGetShaderInfoLog(shaderID, m, &m, l);
            std::cout<<"Compiler Error: "<<l<<std::endl;
            delete[] l;
        }
        return shaderID;
    }
    void add_shader_program(GLuint &shaderProgram, const std::string vs, const std::string fs)
    {
        //компилируем шейдеры и связываем их с программой
        shaderProgram = glCreateProgram(); 
        int vs_code = add_shader(vs,shaderProgram, GL_VERTEX_SHADER);
        int fs_code = add_shader(fs,shaderProgram, GL_FRAGMENT_SHADER);
        glLinkProgram(shaderProgram);

        //печатаем ошибки линковки
        int success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) 
        {
            int m;
            glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &m);
            char* l = new char[m];
            glGetProgramInfoLog(shaderProgram, m, &m, l);
            std::cout<<"Linker Error: "<<l<<std::endl;
            delete[] l;
        }
        glDeleteShader(vs_code);
        glDeleteShader(fs_code);
    }

    void Renderer::Init(SDL_Window *_window, int w, int h)
    {
        window = _window;
        width = w;
        height = h;

        GLfloat vertices[12*3 + FLAGH * FLAGW * 3] = {
            -1.5f, 0.f, -1.5f,
            18.f, 0.f, -1.5f,
            -1.5f, 0.f, 15.f,
            18.f, 0.f, 15.f,
            0.f, 0.f, 0.f,
            0.1f, 0.f, 0.f, 
            0.f, 0.f, 0.1f,
            0.1f,  0.f, 0.1f,
            0.f, 1.5f, 0.f,
            0.1f, 1.5f, 0.f, 
            0.f, 1.5f, 0.1f,
            0.1f, 1.5f, 0.1f,
        };

        for (int i = 0; i < FLAGH * FLAGW; ++i) {
            int col = i / FLAGH;
            int row = i % FLAGH;
            vertices[3 * i + 12 * 3] =  -FLAG_LENGTH / (FLAGW - 1) * col;
            vertices[3 * i + 12 * 3 + 1] =  1.0f + 0.5f / (FLAGH - 1) * row;
            vertices[3 * i + 12 * 3 + 2] =  0.05f;
        }

        GLuint indices[(FLAGH - 1) * (FLAGW - 1) * 2 * 3 + 7 * 2 * 3] = {
            0, 1, 2,
            1, 2, 3,
            4, 5, 7,
            4, 6, 7,
            8, 9, 11,
            8, 10, 11,
            4, 5, 9,
            4, 8, 9,
            4, 6, 8,
            6, 8, 10,
            5, 7, 9,
            7, 9, 11,
            7, 6, 10,
            7, 10, 11,
        };

        for (int i = 0; i < (FLAGH - 1) * (FLAGW - 1); ++i) {
            int offs = 6 * i + 7 * 2 * 3;
            int vid = 8 + i + i / (FLAGH - 1);

            indices[offs] = vid;
            indices[offs + 1] = vid + FLAGH;
            indices[offs + 2] = vid + FLAGH + 1;

            indices[offs + 3] = vid;
            indices[offs + 4] = vid + 1;
            indices[offs + 5] = vid + FLAGH + 1;
        }

        GLfloat g_color_buffer_data[12*3 + FLAGH * FLAGW * 3] = {
            0.f, 0.4f, 0.f,
            0.f, 0.4f, 0.f,
            0.f, 0.4f, 0.f,
            0.f, 0.4f, 0.f,
            0.4f, 0.2f, 0.1f,
            0.4f, 0.2f, 0.1f,
            0.4f, 0.2f, 0.1f,
            0.4f, 0.2f, 0.1f,
            0.4f, 0.2f, 0.1f,
            0.4f, 0.2f, 0.1f,
            0.4f, 0.2f, 0.1f,
            0.4f, 0.2f, 0.1f,
        };


        for (int i = 0; i < FLAGH * FLAGW; ++i) {
            g_color_buffer_data[3 * i + 12 * 3] = 1.f;
            g_color_buffer_data[3 * i + 12 * 3 + 1] = 1.f;
            g_color_buffer_data[3 * i + 12 * 3 + 2] = 1.f;
        }

        GLint vtype[12 + FLAGH * FLAGW] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        for (int i = 0; i < FLAGH * FLAGW; ++i) {
            vtype[i + 12] = 1;
        }

        GLfloat texCoord[12*2 + FLAGH * FLAGW * 2] = {
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
        };

        for (int i = 0; i < FLAGH * FLAGW; ++i) {
            int col = i / FLAGH;
            int row = i % FLAGH;
            texCoord[2 * i + 12 * 2] = col * 1.0f / (FLAGW - 1);
            texCoord[2 * i + 12 * 2 + 1] = 1 - row * 1.0f / (FLAGH - 1);
        }


        // 1. Создаем буферы
        glGenBuffers(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &colorbuffer);
        glGenBuffers(1, &typebuffer);
        glGenBuffers(1, &texbuffer);
        glGenTextures(1, &texture);

        glBindVertexArray(VAO);
    // 2. Копируем наши вершины в буфер для OpenGL
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. Копируем наши индексы в в буфер для OpenGL
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 3. Устанавливаем указатели на вершинные атрибуты
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0); 
    
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

        glBindBuffer(GL_ARRAY_BUFFER, typebuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vtype), vtype, GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, typebuffer);
        glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, 1 * sizeof(GLint), (GLvoid*)0);

        glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

        int wi, he, ch;
        unsigned char* image = stbi_load("assets/russia-flag-medium.jpg", &wi, &he, &ch, STBI_rgb);
        if (image == NULL)
            std::cout << "Cannot load texture\n";
        std::cout << stbi_failure_reason() << '\n';
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wi, he, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

    // 4. Отвязываем VAO (НЕ EBO)
    glBindVertexArray(0);
    time_beg = std::chrono::high_resolution_clock::now();

    add_shader_program(shaderProgram,"shaders/simple.vert","shaders/simple.frag");
    }

    void Renderer::Render()
    {
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
        glm::mat4 View = glm::lookAt(
            glm::vec3(8, 20, -14),
            glm::vec3(8,0,6),
            glm::vec3(0,1,0)
        );
        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 mvp = Projection * View * Model;
        GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        glUseProgram(shaderProgram);
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> time_span = t2 - time_beg;
        glUniform3f(glGetUniformLocation(shaderProgram, "color"),0,1,0);
        int dtime = (int)(time_span.count() / 80);
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), dtime - (floor((float)dtime / (2 * M_PI)) * 2 * M_PI));
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);
        glDrawElementsInstanced(GL_TRIANGLES, (FLAGH - 1) * (FLAGW - 1) * 2 * 3 + 6 * 2 * 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)), 120);
        glBindVertexArray(0);
    }
    void Renderer::Close()
    {
        glDeleteBuffers(1,&VAO);
        glDeleteBuffers(1,&VBO);
        glDeleteBuffers(1,&EBO);
        glDeleteProgram(shaderProgram);
    }