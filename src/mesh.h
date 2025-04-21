#ifndef MESH_H
#define MESH_H
#include "list.h"
typedef struct Mesh
{
    float *vertices;
    int *indices;
    int vertex_count;
    int indice_count;
}Mesh;

typedef struct MeshBuilder
{
    float *vertices;
    int *indices;
    int vertex_count;
    int indice_count;
}MeshBuilder;

MeshBuilder *init_builder(float *vertices, int *indices, int vertex_count, int indice_count);
Mesh *generate_mesh(MeshBuilder *builder);
void print_mesh(Mesh *mesh);


#endif