#include "terrain.h"
#include "glad/glad.h"
#include <stdlib.h>
#define CARTESIAN_COORDINATE_SIZE 3

Terrain *init_terrain(float x, float y, float z, int sectionwidth, int sectiondepth, int sectionx_ct, int sectiony_ct)
{
    Terrain *result = malloc(sizeof(Terrain));

    int quads_x = sectionx_ct;
    int quads_y = sectiony_ct;
    int quads_total = quads_x * quads_y;

    int vertices_per_quad = 6; // 2 triangles per quad * 3 vertices
    int floats_per_vertex = CARTESIAN_COORDINATE_SIZE;

    result->vertex_count = quads_total * vertices_per_quad;
    result->indice_count = result->vertex_count;

    result->vertices = malloc(sizeof(float) * result->vertex_count * floats_per_vertex);
    result->indices = malloc(sizeof(int) * result->indice_count);

    int vi = 0;
    int ii = 0;

    for (int xq = 0; xq < quads_x; ++xq)
    {
        for (int yq = 0; yq < quads_y; ++yq)
        {
            // Corners of the quad
            float x0 = x + xq * sectionwidth;
            float x1 = x + (xq + 1) * sectionwidth;
            float y0 = y + yq * sectiondepth;
            float y1 = y + (yq + 1) * sectiondepth;

            // Triangle 1
            float tri1[] = {
                x0, y0, z,
                x1, y0, z,
                x1, y1, z};
            // Triangle 2
            float tri2[] = {
                x0, y0, z,
                x1, y1, z,
                x0, y1, z};

            // Copy vertices
            for (int i = 0; i < 9; ++i)
                result->vertices[vi++] = tri1[i];
            for (int i = 0; i < 9; ++i)
                result->vertices[vi++] = tri2[i];

            // Indices (just 0..n, since we duplicate vertices)
            for (int i = 0; i < 6; ++i)
                result->indices[ii] = ii++;
        }
    }

    result->has_texture = false;
    return result;
}

void generate_terrain_gpu_objects(Terrain *mesh)
{
    int floats_per_vertex = 3 + 3 + (mesh->has_texture ? 2 : 0); // pos + normal + [uv]

    glGenVertexArrays(1, &mesh->VAO);
    glBindVertexArray(mesh->VAO);

    // VBO (vertex attributes: pos + normal + tex)
    glGenBuffers(1, &mesh->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * floats_per_vertex * sizeof(float), mesh->vertices, GL_STATIC_DRAW);

    // EBO (indices)
    glGenBuffers(1, &mesh->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indice_count * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

    // Position (layout location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, floats_per_vertex * sizeof(float), (void *)(0));
    glEnableVertexAttribArray(0);

    // Normal (layout location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, floats_per_vertex * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture Coords (layout location = 2)
    if (mesh->has_texture)
    {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, floats_per_vertex * sizeof(float), (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    glBindVertexArray(0); // unbind VAO
}