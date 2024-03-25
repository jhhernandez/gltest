#include <GL/glew.h>
#include <GL/glu.h>
#include <iostream>
#include <string>

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "src/shader.h"
#include "src/load_texture.cpp"
#include "src/camera.h"

#include "src/cube.h"

using std::cout;
using std::endl;

bool processInput(const Shader& shader, Camera& camera);

const char* TITLE = "LearnOpenGL";

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float yaw = -90.0f;
float pitch = 0.0f;

float fov = 45.0f;

int main()
{
    GLint width = 800;
    GLint height = 600;
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL could not initialize! SDL_Error:" << SDL_GetError() << endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    window = SDL_CreateWindow(TITLE, width, height, SDL_WINDOW_OPENGL);

    if (window == nullptr)
    {
        cout << "Window could not be created! SDL_Error:" << SDL_GetError() << endl;
        return 1;
    }

    context = SDL_GL_CreateContext(window);

    if (context == nullptr)
    {
        cout << "OpenGL context could not be created! SDL Error:" << SDL_GetError() << endl;
        return 1;
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();

    if (glewError != GLEW_OK){
        cout << "Error initializing GLEW!" << glewGetErrorString(glewError) << endl;
        return 1;
    }

    if (SDL_GL_SetSwapInterval(1) < 0)
    {
        cout << "Warning: Unable to set VSync! SDL Error:" << SDL_GetError() << endl;
        return 1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);
    Shader shader("../shaders/tex_shader.vs", "../shaders/tex_shader.fs");
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    // Shader shader2("../shaders/tex_shader.vs", "../shaders/tex_shader.fs");

    // unsigned int indices[] = {
    //     0, 1, 3, // first triangle
    //     1, 2, 3  // second triangle
    // };
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // Create buffers
    GLuint VBO[2], VAO[2], EBO[2];

    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // glBindVertexArray(VAO[1]);
    //
    // glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    //
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    //
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);

    GLuint textures[2];
    glGenTextures(2, textures);

    load_texture(textures[0], GL_RGB, false, "../assets/container.jpg");
    load_texture(textures[1], GL_RGBA, true, "../assets/awesomeface.png");

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shader.use();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    // glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // shader.setMat4("model", model);
    shader.setMat4("view", camera.GetViewMatrix());
    shader.setMat4("projection", glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f));
    shader.setFloat("mixPercentage", 0.2f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    // shader2.use();
    // shader2.setInt("texture1", 0);
    // shader2.setInt("texture2", 1);
    // shader2.setFloat("mixPercentage", 0.2f);
    //
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, textures[0]);
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, textures[1]);

    glEnable(GL_DEPTH_TEST);
    const float radius = 10.0f;

    while (!processInput(shader, camera))
    {
        // clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = SDL_GetTicks() / 100.0f;

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // float offset = sin(ticks / 1000.0f) / 2.0f;
        // GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        // shader.setFloat("offset", offset);

        // bind textures
        shader.use();

        // glm::mat4 transform = glm::mat4(1.0f);
        // GLuint transformLoc = glGetUniformLocation(shader.ID, "transform");
        // transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        // transform = glm::rotate(transform, static_cast<float>(ticks / 1000.0f) + 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        // model = glm::rotate(model, glm::radians(0.5f), glm::vec3(0.5f, 1.0f, 0.0f));
        // shader.setMat4("model", model);
        shader.setMat4("view", camera.GetViewMatrix());
        shader.setMat4("projection", glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f));

        glBindVertexArray(VAO[0]);

        for(int i = 0; i < 10; ++i) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i % 3 == 0) {
                model = glm::rotate(model, glm::radians(currentFrame * 10.0f), glm::vec3(1.0f, 0.3f, 0.5f));
            }
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // shader2.use();
        //
        // glm::mat4 transform2 = glm::mat4(1.0f);
        // GLuint transformLoc2 = glGetUniformLocation(shader2.ID, "transform");
        // transform2 = glm::translate(transform2, glm::vec3(-0.5f, 0.5f, 0.0f));
        // transform2 = glm::scale(transform2, glm::vec3(
        //     abs(sin(static_cast<float>(ticks / 1000.0f))),
        //     abs(sin(static_cast<float>(ticks / 1000.0f))),
        //     0.0f)
        // );
        // glUniformMatrix4fv(transformLoc2, 1, GL_FALSE, glm::value_ptr(transform2));
        //
        // glBindVertexArray(VAO[1]);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

bool processInput(const Shader& shader, Camera& camera)
{
    SDL_Event e;
    bool quit = false;
    GLfloat mix;

    while(SDL_PollEvent(&e))
    {
        mix = shader.getFloat("mixPercentage");

        switch (e.type)
        {
            case SDL_EVENT_KEY_DOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_PAGEUP:
                        shader.setFloat("mixPercentage", mix >= 1.0 ? 1.0 : mix + 0.1);
                        break;
                    case SDLK_PAGEDOWN:
                        shader.setFloat("mixPercentage", mix <= 0.1 ? 0.0 : mix - 0.1);
                        break;
                    case SDLK_w:
                        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
                        break;
                    case SDLK_a:
                        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
                        break;
                    case SDLK_s:
                        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
                        break;
                    case SDLK_d:
                        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
                        break;
                }
                break;
            case SDL_EVENT_MOUSE_MOTION: {
                camera.ProcessMouseMovement(e.motion.xrel, e.motion.yrel);
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL: {
                camera.ProcessMouseScroll(e.wheel.y);
                break;
            }
            case SDL_EVENT_QUIT:
                quit = true;
                break;
        }
    }

    return quit;
}

