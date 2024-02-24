#include <GL/glew.h>
#include <GL/glu.h>
#include <iostream>
#include <string>

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL.h>

using std::cout;
using std::endl;

bool processInput();
void createShaders();

const char* TITLE = "LearnOpenGL";
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

GLuint shaderProgram;



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

    window = SDL_CreateWindow(
        TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );

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

    createShaders();

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    GLuint VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO, allowed but not necessary in this case
    glBindVertexArray(0); // unbind VAO, allowed but not necessary in this case

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!processInput())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

bool processInput()
{
    SDL_Event e;
    bool quit = false;

    while(SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_KEYDOWN:
                if (e.key.keysym.sym != SDLK_ESCAPE)
                {
                    break;
                }
            case SDL_QUIT:
                quit = true;
                break;
        }
    }

    return quit;
}

void createShaders()
{
    constexpr uint logSize = 512;
    int success;
    char infoLog[logSize];
    shaderProgram = glCreateProgram();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, logSize, nullptr, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED:" << infoLog << endl;
    }

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, logSize, nullptr, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED:" << infoLog << endl;
    }

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, logSize, nullptr, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED:" << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
