#include "mesh.h"
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h> // Assume you're using cglm for math functions
#include <stdio.h>
#define EPSILON 0.001

MeshBuilder *init_builder(float *vertices, int *indices, int vertex_count, int indice_count)
{
    MeshBuilder *builder = malloc(sizeof(MeshBuilder));
    builder->vertices = vertices;
    builder->vertex_count = vertex_count;
    builder->indice_count = indice_count;
    builder->indices = indices;
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

    float **triangles = malloc(sizeof(float *) * (builder->indice_count / 3));
    int triangles_top = 0;

    int workingcount = 0;

    for (int i = 0; i < builder->indice_count; i += 3)
    {
        float *working_triangle = malloc(sizeof(float) * 9); // Store 3 vertices (each vertex 3 components)

        for (int j = 0; j < 3; ++j)
        {
            memcpy(working_triangle + j * 3, vertices + (indices[i + j]*3), sizeof(float) * 3);
            workingcount += 18; // Add space for another normal for each vertex (9) + the duplicate position data (9)
            //(3 points per vertex + 3 points per normal) * 3 points per triangle = 6 * 3 = 18
        }
        triangles[triangles_top++] = working_triangle;
    }

    float *result_vertices = malloc(sizeof(float) * workingcount);
    int *result_indices = malloc(sizeof(int) * workingcount / 6); // Every vertex will be used at least once as theres a unique normal for each vertex. /6 so we ignore the spacing in the floats
    int result_vertices_top = 0;
    int result_indices_top = 0;

    for (int i = 0; i < triangles_top; ++i)
    {
        float *triangle = triangles[i];
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

            float *other_triangle = triangles[j];

            // Ray-triangle intersection test
            if (ray_intersects_triangle(ray_origin, ray_direction, other_triangle))
            {
                intersection_count++;
            }
        }

        // Inside/Outside test: Count intersections
        if (intersection_count % 2 == 1)
        {
            // Odd number of intersections → Inside the mesh
            // Use the opposite normal (flip the normal direction)
            glm_vec3_negate(normal);
        }
        // Else the point is outside the mesh, use the current normal



        int firstitem = result_vertices_top / 6;
        for (int j = 0; j < 3; ++j)
        {
            memcpy(result_vertices + result_vertices_top, triangle + (j * 3), sizeof(float) * 3); // Copy vertex position
            memcpy(result_vertices + result_vertices_top + 3, normal, sizeof(float) * 3);         // Copy shared normal
            result_vertices_top += 6;
        }
        // Add indices for this triangle (3 sequential new vertices)
        for (int j = 0; j < 3; ++j)
        {
            result_indices[result_indices_top++] = firstitem + j;
        }
    }
    Mesh *result = init_mesh();
    result->indices = result_indices;
    result->vertices = result_vertices;
    result->vertex_count = result_vertices_top / 6;
    result->indice_count = result_indices_top;


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
    printf("Vertices (Position + Normal):\n");
    for (int i = 0; i < mesh->vertex_count; ++i)
    {
        float *v = mesh->vertices + i * 6;
        printf("Vertex %2d | Pos: (% .2f, % .2f, % .2f)  Norm: (% .2f, % .2f, % .2f)\n",
               i, v[0], v[1], v[2], v[3], v[4], v[5]);
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