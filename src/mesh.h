#ifndef MESH_H
#define MESH_H
#include "list.h"
typedef struct Mesh
{
    float *vertices;
    int *indices;
    int vertex_count;
    int indice_count;
    bool has_texture;
    unsigned int VAO, VBO, TBO, EBO;
}Mesh;

typedef struct MeshBuilder
{
    float *vertices;
    float *tex_coords;
    int *indices;
    int vertex_count;
    int indice_count;
}MeshBuilder;

MeshBuilder *init_builder(float *vertices, int *indices, int vertex_count, int indice_count, float* tex_coords);
Mesh *generate_mesh(MeshBuilder *builder);
void generate_gpu_objects(Mesh *mesh);

void print_mesh(Mesh *mesh);


#endif