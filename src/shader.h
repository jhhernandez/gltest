#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>

using std::string;
using std::ifstream;
using std::cout;
using std::endl;
using std::stringstream;

class Shader
{
public:
    // the program ID
    GLuint ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath) {
      // 1. retrieve the vertex/fragment source code from filePath
      string vertexCode;
      string fragmentCode;
      ifstream vShaderFile;
      ifstream fShaderFile;
      // ensure ifstream objects can throw exceptions:
      vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
      fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
      try
      {
          // open files
          vShaderFile.open(vertexPath);
          fShaderFile.open(fragmentPath);
          stringstream vShaderStream, fShaderStream;
          // read file's buffer contents into streams
          vShaderStream << vShaderFile.rdbuf();
          fShaderStream << fShaderFile.rdbuf();
          // close file handlers
          vShaderFile.close();
          fShaderFile.close();
          // convert stream into string
          vertexCode   = vShaderStream.str();
          fragmentCode = fShaderStream.str();
      }
      catch(ifstream::failure e)
      {
          cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
      }
      const char* vShaderCode = vertexCode.c_str();
      const char* fShaderCode = fragmentCode.c_str();

      // 2. compile shaders
      GLuint vertex, fragment;
      int success;
      char infoLog[512];

      // vertex Shader
      vertex = glCreateShader(GL_VERTEX_SHADER);
      glShaderSource(vertex, 1, &vShaderCode, nullptr);
      glCompileShader(vertex);
      // print compile errors if any
      glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

      if(!success)
      {
          glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
          cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED " << infoLog << endl;
      };

      // similiar for Fragment Shader
      fragment = glCreateShader(GL_FRAGMENT_SHADER);
      glShaderSource(fragment, 1, &fShaderCode, nullptr);
      glCompileShader(fragment);
      glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

      if(!success)
      {
          glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
          cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED " << infoLog << endl;
      };

      // shader Program
      ID = glCreateProgram();
      glAttachShader(ID, vertex);
      glAttachShader(ID, fragment);
      glLinkProgram(ID);
      // print linking errors if any
      glGetProgramiv(ID, GL_LINK_STATUS, &success);
      if(!success)
      {
          glGetProgramInfoLog(ID, 512, nullptr, infoLog);
          cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED " << infoLog << endl;
      }

      // delete the shaders as they're linked into our program now and no longer necessary
      glDeleteShader(vertex);
      glDeleteShader(fragment);
    }
    // use/activate the shader
    void use() {
       glUseProgram(ID);
    }
    // utility uniform functions
    void setBool(const string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
};

#endif
