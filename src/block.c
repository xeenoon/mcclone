#include "block.h"
#include <glad/glad.h> // or appropriate OpenGL header
#include <stdlib.h>
#include <string.h>

BlockTypeData block_data[BLOCK_COUNT];

void setup_block_data()
{
    // Unit cube geometry (position, normal, texcoords)
    static float dirt_vertices[] = {
        // Positions         // Normals         // Tex Coords
        // Front face
        0, 0, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 0, 0, 1, 1, 0,
        1, 1, 1, 0, 0, 1, 1, 1,
        0, 1, 1, 0, 0, 1, 0, 1,
        // Back face
        0, 0, 0, 0, 0, -1, 0, 0,
        1, 0, 0, 0, 0, -1, 1, 0,
        1, 1, 0, 0, 0, -1, 1, 1,
        0, 1, 0, 0, 0, -1, 0, 1,
        // Left face
        0, 0, 0, -1, 0, 0, 0, 0,
        0, 0, 1, -1, 0, 0, 1, 0,
        0, 1, 1, -1, 0, 0, 1, 1,
        0, 1, 0, -1, 0, 0, 0, 1,
        // Right face
        1, 0, 0, 1, 0, 0, 0, 0,
        1, 0, 1, 1, 0, 0, 1, 0,
        1, 1, 1, 1, 0, 0, 1, 1,
        1, 1, 0, 1, 0, 0, 0, 1,
        // Top face
        0, 1, 0, 0, 1, 0, 0, 0,
        0, 1, 1, 0, 1, 0, 1, 0,
        1, 1, 1, 0, 1, 0, 1, 1,
        1, 1, 0, 0, 1, 0, 0, 1,
        // Bottom face
        0, 0, 0, 0, -1, 0, 0, 0,
        0, 0, 1, 0, -1, 0, 1, 0,
        1, 0, 1, 0, -1, 0, 1, 1,
        1, 0, 0, 0, -1, 0, 0, 1};

    static int dirt_indices[] = {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,    // Left
        12, 13, 14, 14, 15, 12, // Right
        16, 17, 18, 18, 19, 16, // Top
        20, 21, 22, 22, 23, 20  // Bottom
    };

    BlockTypeData *dirt = &block_data[Dirt];
    dirt->vertices = malloc(sizeof(dirt_vertices));
    memcpy(dirt->vertices, dirt_vertices, sizeof(dirt_vertices));
    dirt->vertex_count = sizeof(dirt_vertices) / sizeof(float) / VERTEX_SIZE;

    dirt->indices = malloc(sizeof(dirt_indices));
    memcpy(dirt->indices, dirt_indices, sizeof(dirt_indices));
    dirt->indice_count = sizeof(dirt_indices) / sizeof(int);

    dirt->texture_path = "assets/dirt.jpg";

    // OpenGL buffer setup
    glGenVertexArrays(1, &dirt->VAO);
    glGenBuffers(1, &dirt->VBO);
    glGenBuffers(1, &dirt->EBO);

    glBindVertexArray(dirt->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, dirt->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dirt_vertices), dirt_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dirt->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(dirt_indices), dirt_indices, GL_STATIC_DRAW);

    // Vertex attributes: position (0), normal (1), texcoords (2)
    int stride = VERTEX_SIZE * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // unbind

    // Dummy shader setup – replace this with actual shader compilation later
    dirt->shader = init_shader(GL_VERTEX_SHADER, dirt->texture_path);
    compile_shader(dirt->shader);
}

BlockInstance *init_block(int x, int y, int z, BlockType type)
{
    BlockInstance *result = malloc(sizeof(BlockInstance));
    result->x = x;
    result->y = y;
    result->z = z;
    result->type = type;
    return result;
}