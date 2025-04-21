#ifndef SHADER_H
#define SHADER_H
#include <stdint.h>
typedef struct Shader
{
    unsigned int type;
    unsigned int shaderptr;
}Shader;
Shader *init_shader(unsigned int type, char *source_fp);
void compile_shader(Shader *shader);
void free_shader(Shader *shader);
#endif