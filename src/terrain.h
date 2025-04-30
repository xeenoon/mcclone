#ifndef TERRAIN_H
#define TERRAIN_H
#include <stdbool.h>
typedef struct Terrain
{
    float *vertices;
    int *indices;
    int vertex_count;
    int indice_count;
    bool has_texture;
    int sectionx_ct, sectiony_ct;
    unsigned int VAO, VBO, TBO, EBO;
}Terrain;

Terrain *init_terrain(float x, float y, float z, int sectionwidth, int sectiondepth, int sectionx_ct, int sectiony_ct);
void generate_terrain_gpu_objects(Terrain *mesh);

#endif