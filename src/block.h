#ifndef BLOCK_H
#define BLOCK_H
#include "shader.h"

#define BLOCK_COUNT 1
#define VERTEX_SIZE 8

typedef enum {
    Dirt,
} BlockType;

typedef struct {
    float *vertices;
    int *indices;
    int vertex_count;
    int indice_count;

    unsigned int VAO, VBO, EBO;
    Shader *shader;
    char *texture_path;
} BlockTypeData;

typedef struct {
    int x, y, z;
    BlockType type;
} BlockInstance;

extern BlockTypeData block_data[BLOCK_COUNT];
void setup_block_data();
BlockInstance *init_block(int x, int y, int z, BlockType type); 


#endif
