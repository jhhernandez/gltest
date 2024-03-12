#include <iostream>

#include <GL/glew.h>

#include "image.h"

using std::cout;
using std::endl;

int load_texture(const GLuint texture, GLint internalFormat, bool flip, const char* filename)
{
    int texWidth, texHeight, nrChannels;
    int loaded = 0;
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(flip);
    u_char* data = stbi_load(filename, &texWidth, &texHeight, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, internalFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        loaded = 0;
    }
    else
    {
        cout << "Failed to load texture" << endl;
        loaded = -1;
    }

    stbi_image_free(data);

    return loaded;
}
