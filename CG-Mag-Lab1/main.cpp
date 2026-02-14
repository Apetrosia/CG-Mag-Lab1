#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

GLuint Program, VAO, VBO, EBO;
GLuint modelLoc;

GLuint texture;

GLfloat angleX1 = 0.0f, angleY1 = 0.0f, angleZ1 = 0.0f;
GLfloat angleX2 = 0.0f, angleY2 = 0.0f, angleZ2 = 0.0f;
GLfloat step_angleX1 =  0.015f, step_angleY1 =  0.015f, step_angleZ1 =  0.015f;
GLfloat step_angleX2 = -0.015f, step_angleY2 = -0.015f, step_angleZ2 = -0.015f;

GLfloat transformMatrix[16];

std::vector<GLuint> indices;

void CreateTransformMatrix(float angleX, float angleY, float angleZ)
{
    GLfloat rx[16] = {
        1,0,0,0,
        0,cos(angleX),-sin(angleX),0,
        0,sin(angleX), cos(angleX),0,
        0,0,0,1
    };

    GLfloat ry[16] = {
        cos(angleY),0,sin(angleY),0,
        0,1,0,0,
        -sin(angleY),0,cos(angleY),0,
        0,0,0,1
    };

    GLfloat rz[16] = {
        cos(angleZ),-sin(angleZ),0,0,
        sin(angleZ), cos(angleZ),0,0,
        0,0,1,0,
        0,0,0,1
    };

    GLfloat s[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    GLfloat t1[16], t2[16];

    for (int i = 0; i < 16; i++)
        t1[i] = ry[i % 4] * rx[i / 4 * 4] +
        ry[i % 4 + 4] * rx[i / 4 * 4 + 1] +
        ry[i % 4 + 8] * rx[i / 4 * 4 + 2] +
        ry[i % 4 + 12] * rx[i / 4 * 4 + 3];

    for (int i = 0; i < 16; i++)
        t2[i] = rz[i % 4] * t1[i / 4 * 4] +
        rz[i % 4 + 4] * t1[i / 4 * 4 + 1] +
        rz[i % 4 + 8] * t1[i / 4 * 4 + 2] +
        rz[i % 4 + 12] * t1[i / 4 * 4 + 3];

    for (int i = 0; i < 16; i++)
        transformMatrix[i] =
        s[i % 4] * t2[i / 4 * 4] +
        s[i % 4 + 4] * t2[i / 4 * 4 + 1] +
        s[i % 4 + 8] * t2[i / 4 * 4 + 2] +
        s[i % 4 + 12] * t2[i / 4 * 4 + 3];
}

GLuint LoadTexture(const char* file)
{
    sf::Image img;
    if (!img.loadFromFile(file)) return 0;
    img.flipVertically();

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        img.getSize().x, img.getSize().y,
        0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());

    glGenerateMipmap(GL_TEXTURE_2D);
    return tex;
}

std::vector<GLfloat> vertices = {
    // coord              color               tex
    // Задняя грань куба
    -0.4f, -0.4f,  0.4f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // Нижняя левая (красный)
     0.4f, -0.4f,  0.4f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Нижняя правая (красный)
     0.4f,  0.4f,  0.4f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // Верхняя правая (красный)
    -0.4f,  0.4f,  0.4f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Верхняя левая (красный)

    // Передняя грань куба
    -0.4f, -0.4f, -0.4f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Нижняя левая (красный)
     0.4f, -0.4f, -0.4f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // Нижняя правая (красный)
     0.4f,  0.4f, -0.4f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Верхняя правая (красный)
    -0.4f,  0.4f, -0.4f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,  // Верхняя левая (красный)

    // Левая грань куба
    -0.4f, -0.4f,  0.4f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Нижняя левая (зеленый)
    -0.4f, -0.4f, -0.4f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Нижняя левая (зеленый)
    -0.4f,  0.4f, -0.4f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Верхняя левая (зеленый)
    -0.4f,  0.4f,  0.4f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Верхняя левая (зеленый)

    // Правая грань куба
     0.4f, -0.4f, -0.4f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Нижняя правая (зеленый)
     0.4f, -0.4f,  0.4f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Нижняя правая (зеленый)
     0.4f,  0.4f,  0.4f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Верхняя правая (зеленый)
     0.4f,  0.4f, -0.4f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Верхняя правая (зеленый)

     // Верхняя грань куба
    -0.4f,  0.4f, -0.4f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Верхняя левая (синий)
     0.4f,  0.4f, -0.4f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Верхняя правая (синий)
     0.4f,  0.4f,  0.4f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Верхняя правая (синий)
    -0.4f,  0.4f,  0.4f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // Верхняя левая (синий)

    // Нижняя грань куба
    -0.4f, -0.4f, -0.4f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // Нижняя левая (синий)
    -0.4f, -0.4f,  0.4f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Нижняя левая (синий)
     0.4f, -0.4f,  0.4f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Нижняя правая (синий)
     0.4f, -0.4f, -0.4f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f // Нижняя правая (синий)
};

const char* VS = R"(
#version 330 core
layout(location=0) in vec3 coord;
layout(location=1) in vec3 color;
layout(location=2) in vec2 tex;

uniform mat4 model;
out vec2 vTex;
out vec3 vColor;

void main() {
    gl_Position = model * vec4(coord,1.0);
    vTex = tex;
    vColor = color;
}
)";

const char* FS = R"(
#version 330 core
in vec2 vTex;
in vec3 vColor;
out vec4 color;

uniform sampler2D texture;
uniform bool useTexture;

void main() {
    if (useTexture)
        color = texture(texture,vTex);
    else
        color = vec4(vColor,1.0);
}
)";

void InitShader()
{
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &VS, nullptr);
    glCompileShader(v);

    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &FS, nullptr);
    glCompileShader(f);

    Program = glCreateProgram();
    glAttachShader(Program, v);
    glAttachShader(Program, f);
    glLinkProgram(Program);

    glDeleteShader(v);
    glDeleteShader(f);
}

void InitBuffers()
{
    indices = {
        // Задняя грань
        0, 1, 2,
        2, 3, 0,
        // Передяя грань
        4, 5, 6,
        6, 7, 4,
        // Левая грань
        8, 9, 10,
        10, 11, 8,
        // Правая грань
        12, 13, 14,
        14, 15, 12,
        // Верхняя грань
        16, 17, 18,
        18, 19, 16,
        // Нижняя грань
        20, 21, 22,
        22, 23, 20
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(GLfloat),
        vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(), GL_STATIC_DRAW);

    // coord
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // tex
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);


    glBindVertexArray(0);
}

void Draw(bool useTex, GLfloat angleX, GLfloat angleY, GLfloat angleZ)
{
    glUseProgram(Program);
    CreateTransformMatrix(angleX, angleY, angleZ);

    modelLoc = glGetUniformLocation(Program, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transformMatrix);

    glUniform1i(glGetUniformLocation(Program, "useTexture"), useTex);

    if (useTex)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(Program, "texture"), 0);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

int main()
{
    sf::Window window(sf::VideoMode(1600, 800), "Two cubes",
        sf::Style::Default, sf::ContextSettings(24));

    window.setVerticalSyncEnabled(true);
    glewInit();

    texture = LoadTexture("pic2.jpg");

    InitShader();
    InitBuffers();
    glEnable(GL_DEPTH_TEST);

    while (window.isOpen())
    {
        sf::Event e;
        angleY1 += step_angleX1;
        angleX1 += step_angleY1;
        angleZ1 += step_angleZ1;

        angleY2 += step_angleX2;
        angleX2 += step_angleY2;
        angleZ2 += step_angleZ2;

        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::KeyPressed)
            {
                switch (e.key.code)
                {
                case sf::Keyboard::W:
					step_angleY1 += 0.005f;
                    break;
                case sf::Keyboard::S:
                    step_angleY1 -= 0.005f;
                    break;
                case sf::Keyboard::D:
                    step_angleX1 += 0.005f;
                    break;
                case sf::Keyboard::A:
                    step_angleX1 -= 0.005f;
                    break;
                case sf::Keyboard::E:
                    step_angleZ1 += 0.005f;
                    break;
                case sf::Keyboard::Q:
                    step_angleZ1 -= 0.005f;
                    break;
                case sf::Keyboard::Up:
                    step_angleY2 += 0.005f;
                    break;
                case sf::Keyboard::Down:
                    step_angleY2 -= 0.005f;
                    break;
                case sf::Keyboard::Right:
                    step_angleX2 += 0.005f;
                    break;
                case sf::Keyboard::Left:
                    step_angleX2 -= 0.005f;
                    break;
                case sf::Keyboard::L:
                    step_angleZ2 += 0.005f;
                    break;
                case sf::Keyboard::K:
                    step_angleZ2 -= 0.005f;
                    break;
                case sf::Keyboard::R:
					angleX1 = 0.0f;
					angleY1 = 0.0f;
                    angleZ1 = 0.0f;
                    angleX2 = 0.0f;
                    angleY2 = 0.0f;
                    angleZ2 = 0.0f;
                    step_angleX1 = 0.0f;
                    step_angleY1 = 0.0f;
                    step_angleZ1 = 0.0f;
                    step_angleX2 = 0.0f;
                    step_angleY2 = 0.0f;
                    step_angleZ2 = 0.0f;
                    break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Левый куб
        glViewport(0, 0, 800, 800);
        Draw(true, angleX1, angleY1, angleZ1);

        // Правый куб
        glViewport(800, 0, 800, 800);
        Draw(false, angleX2, angleY2, angleZ2);

        window.display();

    }
}
