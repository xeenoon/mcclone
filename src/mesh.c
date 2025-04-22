#include "mesh.h"
#include "glad/glad.h"
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h> // Assume you're using cglm for math functions
#include <stdio.h>
#define EPSILON 0.001

MeshBuilder *init_builder(float *vertices, int *indices, int vertex_count, int indice_count, float* tex_coords)
{
    MeshBuilder *builder = malloc(sizeof(MeshBuilder));
    builder->vertices = vertices;
    builder->vertex_count = vertex_count;
    builder->indice_count = indice_count;
    builder->indices = indices;
    builder->tex_coords = tex_coords;
    return builder;
}
Mesh *init_mesh()
{
    Mesh *result = malloc(sizeof(Mesh));
    result->indices = NULL;
    result->vertices = NULL;
    result->vertex_count = -1;
    result->indice_count = -1;
    return result;
}
// Function to calculate the normal of a triangle given its vertices
void calculate_normal(float *triangle, float *normal)
{
    vec3 v0, v1, v2;
    // Extract vertices from the triangle array
    glm_vec3_copy(triangle, v0);     // First vertex (v0)
    glm_vec3_copy(triangle + 3, v1); // Second vertex (v1)
    glm_vec3_copy(triangle + 6, v2); // Third vertex (v2)

    // Calculate the two edge vectors
    vec3 edge1, edge2;
    glm_vec3_sub(v1, v0, edge1); // Edge 1 (v1 - v0)
    glm_vec3_sub(v2, v0, edge2); // Edge 2 (v2 - v0)

    // Calculate the cross product (normal to the triangle)
    glm_vec3_cross(edge1, edge2, normal);
    glm_vec3_normalize(normal); // Normalize the normal
}

// Ray-triangle intersection (using Möller–Trumbore algorithm)
int ray_intersects_triangle(float *orig, float *dir, float *triangle)
{
    vec3 v0, v1, v2, e1, e2, h, s, q;
    float a, f, u, v, t;

    glm_vec3_copy(triangle, v0);     // First vertex (v0)
    glm_vec3_copy(triangle + 3, v1); // Second vertex (v1)
    glm_vec3_copy(triangle + 6, v2); // Third vertex (v2)

    glm_vec3_sub(v1, v0, e1); // Edge 1 (v1 - v0)
    glm_vec3_sub(v2, v0, e2); // Edge 2 (v2 - v0)

    glm_vec3_cross(dir, e2, h); // Cross product of ray direction and edge2
    a = glm_vec3_dot(e1, h);    // Dot product of edge1 and h

    if (a > -EPSILON && a < EPSILON)
        return 0; // Ray is parallel to triangle

    f = 1.0f / a;
    glm_vec3_sub(orig, v0, s);  // Vector from triangle origin to ray origin
    u = f * glm_vec3_dot(s, h); // Calculate u parameter
    if (u < 0.0f || u > 1.0f)
        return 0;

    glm_vec3_cross(s, e1, q);     // Cross product of s and e1
    v = f * glm_vec3_dot(dir, q); // Calculate v parameter
    if (v < 0.0f || u + v > 1.0f)
        return 0;

    t = f * glm_vec3_dot(e2, q); // Calculate t parameter
    return (t > EPSILON);        // If t is positive, the ray intersects the triangle
}

Mesh *generate_mesh(MeshBuilder *builder)
{
    float *vertices = builder->vertices;
    int *indices = builder->indices;

    int **triangles = malloc(sizeof(float *) * (builder->indice_count / 3));
    int triangles_top = 0;

    int workingcount = 0;
    bool hastexture = builder->tex_coords == NULL ? false : true;
    int floats_per_vertex = (3 + 3 + (hastexture ? 2 : 0));
    //(3xyz points per vertex + 3xyz points per normal + 2xy points per tex) * 3 points per triangle

    for (int i = 0; i < builder->indice_count; i += 3)
    {
        int *working_triangle = malloc(sizeof(int) * 3); // Store 3 vertices indices (each vertex 3 components)

        for (int j = 0; j < 3; ++j)
        {
            memcpy(working_triangle + j, indices + i + j, sizeof(int) * 3);
            
            workingcount += floats_per_vertex * 3;
        }
        triangles[triangles_top++] = working_triangle;
    }

    float *result_vertices = malloc(sizeof(float) * workingcount);
    int *result_indices = malloc(sizeof(int) * workingcount / floats_per_vertex); // Every vertex will be used at least once as theres a unique normal for each vertex. /6 so we ignore the spacing in the floats
    int result_vertices_top = 0;
    int result_indices_top = 0;

    for (int i = 0; i < triangles_top; ++i)
    {
        float *triangle = malloc(9 * sizeof(float));
        for(int j = 0; j < 3; ++j)
        {
            memcpy(triangle + 3 * j, builder->vertices + triangles[i][j] * 3, sizeof(float) * 3);
        }
        float normal[3];
        calculate_normal(triangle, normal); // Get the normal for this triangle
        if(normal[0] == 0 && normal[1] == 0 && normal[2] == 0)
        {
            printf("suprised picachu :o, degen normal called with triangle of (%f,%f,%f)\n", triangle[0],triangle[1],triangle[2]);
        }

        // Find the center of the triangle (centroid)
        float center[3];
        glm_vec3_add(triangle, triangle + 3, center);
        glm_vec3_add(center, triangle + 6, center);
        glm_vec3_scale(center, 1.0f / 3.0f, center); // Center of the triangle

        // Create a ray from the center of the triangle in the direction of the normal
        vec3 ray_origin, ray_direction;
        glm_vec3_copy(center, ray_origin);    // Ray origin is the triangle's center
        glm_vec3_copy(normal, ray_direction); // Ray direction is the normal
        glm_vec3_normalize(ray_direction);    // Ensure the direction is normalized

        // Raycasting: Check for intersection with other triangles
        int intersection_count = 0;
        for (int j = 0; j < triangles_top; ++j)
        {
            if (i == j)
                continue; // Skip checking with itself

            float *other_triangle = malloc(9 * sizeof(float));
            for(int k = 0; k < 3; ++k)
            {
                memcpy(other_triangle + 3 * k, builder->vertices + triangles[j][k] * 3, sizeof(float) * 3);
            }

            // Ray-triangle intersection test
            if (ray_intersects_triangle(ray_origin, ray_direction, other_triangle))
            {
                intersection_count++;
            }
            free(other_triangle);
        }

        // Inside/Outside test: Count intersections
        if (intersection_count % 2 == 1)
        {
            // Odd number of intersections → Inside the mesh
            // Use the opposite normal (flip the normal direction)
            glm_vec3_negate(normal);
        }
        // Else the point is outside the mesh, use the current normal



        int firstitem = result_vertices_top / floats_per_vertex;
        for (int j = 0; j < 3; ++j)
        {
            memcpy(result_vertices + result_vertices_top, triangle + (j * 3), sizeof(float) * 3); // Copy vertex position
            memcpy(result_vertices + result_vertices_top + 3, normal, sizeof(float) * 3);         // Copy shared normal
            if(hastexture)
            {
                memcpy(result_vertices + result_vertices_top + 6, builder->tex_coords + triangles[i][j]*2, sizeof(float) * 2);         // Copy texture coordinates
            }

            result_vertices_top += floats_per_vertex;
        }
        // Add indices for this triangle (3 sequential new vertices)
        for (int j = 0; j < 3; ++j)
        {
            result_indices[result_indices_top++] = firstitem + j;
        }
        free(triangle);
    }
    Mesh *result = init_mesh();
    result->indices = result_indices;
    result->vertices = result_vertices;
    result->vertex_count = result_vertices_top / floats_per_vertex;
    result->indice_count = result_indices_top;
    result->has_texture = hastexture;


    for (int i = 0; i < triangles_top; ++i)
    {
        void *item = triangles[i];
        free(item);
    }
    free(triangles);

    return result;
}
void free_builder(MeshBuilder *builder)
{
    free(builder);
}

void print_mesh(Mesh *mesh)
{
    int floats_per_vertex = 3 + 3 + (mesh->has_texture ? 2 : 0); // pos + normal + [uv]
    printf("Vertices (Position + Normal + Texture):\n");
    for (int i = 0; i < mesh->vertex_count; ++i)
    {
        float *v = mesh->vertices + i * floats_per_vertex;
        printf("Vertex %2d | Pos: (% .2f, % .2f, % .2f)  Norm: (% .2f, % .2f, % .2f)",
               i, v[0], v[1], v[2], v[3], v[4], v[5]);
        if(mesh->has_texture)
        {
            printf(" Tex (% .2f, % .2f)", v[6], v[7]);
        }
        printf("\n");
    }

    printf("\nIndices (triangles):\n");
    for (int i = 0; i < mesh->indice_count; i += 3)
    {
        printf("Triangle %2d | %d, %d, %d\n", i / 3,
               mesh->indices[i],
               mesh->indices[i + 1],
               mesh->indices[i + 2]);
    }
}
void generate_gpu_objects(Mesh *mesh)
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, floats_per_vertex * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);

    // Normal (layout location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, floats_per_vertex * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture Coords (layout location = 2)
    if (mesh->has_texture) {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, floats_per_vertex * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    glBindVertexArray(0); // unbind VAO
}

