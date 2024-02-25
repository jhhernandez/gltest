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
void createVertexShader(GLuint program);
void createFragmentShader(GLuint program);
void createYellowFragmentShader(GLuint program);

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
const char* fragmentShaderYellowSource= "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
    "}\0";

GLuint shaderProgramOrange;
GLuint shaderProgramYellow;

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

    shaderProgramOrange = glCreateProgram();
    shaderProgramYellow = glCreateProgram();

    createVertexShader(shaderProgramOrange);
    createFragmentShader(shaderProgramOrange);

    glLinkProgram(shaderProgramOrange);

    createVertexShader(shaderProgramYellow);
    createYellowFragmentShader(shaderProgramYellow);

    glLinkProgram(shaderProgramYellow);

/*
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };*/
    float firstTriangle[] = {
        -1.0f, -0.5f, 0.0f, // left, bottom left
        0.0f, -0.5f, 0.0f,  // left, bottom right
        -0.5f,  0.5f, 0.0f, // left, top
    };
    float secondTriangle[] = {
        0.0f, -0.5f, 0.0f, // right, bottom left
        1.0f,  -0.5f, 0.0f, // right, bottom right
        0.5f,  0.5f, 0.0f, // right, top
    };
    GLuint VBO[2], VAO[2];

    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    // glGenBuffers(1, &EBO);

    glBindVertexArray(VAO[0]);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO[1]);

    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // unbind VAO, allowed but not necessary in this case
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO, allowed but not necessary in this case
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!processInput())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgramOrange);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUseProgram(shaderProgramYellow);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
    }

    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteProgram(shaderProgramOrange);
    glDeleteProgram(shaderProgramYellow);

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

void createVertexShader (GLuint program) {
    constexpr uint logSize = 512;
    int success;
    char infoLog[logSize];
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, logSize, nullptr, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED:" << infoLog << endl;
    }

    glAttachShader(program, vertexShader);
    glDeleteShader(vertexShader);
}

void createFragmentShader(GLuint program) {
    constexpr uint logSize = 512;
    int success;
    char infoLog[logSize];
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, logSize, nullptr, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED:" << infoLog << endl;
    }

    glAttachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);
}

void createYellowFragmentShader(GLuint program)
{
    constexpr uint logSize = 512;
    int success;
    char infoLog[logSize];
    GLuint fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShaderYellow, 1, &fragmentShaderYellowSource, nullptr);
    glCompileShader(fragmentShaderYellow);
    glGetShaderiv(fragmentShaderYellow, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShaderYellow, logSize, nullptr, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED:" << infoLog << endl;
    }

    glAttachShader(program, fragmentShaderYellow);
    glDeleteShader(fragmentShaderYellow);
}
