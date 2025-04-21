#include "shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>

Shader *init_shader(unsigned int type, char *source_fp)
{
    unsigned int vertexShader = glCreateShader(type);
    FILE *file = fopen(source_fp, "rb");

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(filesize + 1);
    fread(buffer, 1, filesize, file);
    buffer[filesize] = '\0';
    fclose(file);

    const char *shaderSource = buffer;
    glShaderSource(vertexShader, 1, &shaderSource, NULL);
    //glShaderSource can take in an array of strings to add as your source, for multiple files
    //we are only passing in one string, but we need to pass in an array
    //if we say the length of the array is one and get a pointer to the pointer it works

    free(buffer);

    Shader *result = malloc(sizeof(Shader));
    result->shaderptr = vertexShader;
    result->type = type;
}
void compile_shader(Shader *shader)
{
    glCompileShader(shader->shaderptr);
}

void free_shader(Shader *shader)
{
    glDeleteShader(shader->shaderptr);
    free(shader);
}
