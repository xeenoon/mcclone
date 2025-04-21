#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include "shader.h"
#include "mesh.h"

float cameraSpeed = 2.5f;
vec3 cameraPos = {0.0f, 0.0f, 3.0f};
vec3 cameraFront = {0.0f, 0.0f, -1.0f};
vec3 cameraUp = {0.0f, 1.0f, 0.0f};

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400, lastY = 300;
int firstMouse = 1;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = 0;
    }

    float sensitivity = 0.05f;
    float xoffset = (xpos - lastX) * sensitivity;
    float yoffset = (lastY - ypos) * sensitivity;
    lastX = xpos;
    lastY = ypos;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    vec3 front;
    front[0] = cosf(glm_rad(yaw)) * cosf(glm_rad(pitch));
    front[1] = sinf(glm_rad(pitch));
    front[2] = sinf(glm_rad(yaw)) * cosf(glm_rad(pitch));
    glm_normalize_to(front, cameraFront);
}

void process_input(GLFWwindow* window, float deltaTime) {
    float velocity = cameraSpeed * deltaTime;

    vec3 right;
    glm_cross(cameraFront, cameraUp, right);
    glm_normalize(right);

    vec3 tmp;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm_vec3_scale(cameraFront, velocity, tmp);
        glm_vec3_add(cameraPos, tmp, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm_vec3_scale(cameraFront, velocity, tmp);
        glm_vec3_sub(cameraPos, tmp, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm_vec3_scale(right, velocity, tmp);
        glm_vec3_sub(cameraPos, tmp, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm_vec3_scale(right, velocity, tmp);
        glm_vec3_add(cameraPos, tmp, cameraPos);
    }
}

GLFWwindow* setupwindow() {
    if (!glfwInit()) return NULL;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Cube", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return NULL;
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);

    return window;
}

int main() {

    //FOR TESTING!!!
    float cubeVerts[] = {
        -0.5f, -0.5f, -0.5f, // 0 bottom back left
         0.5f, -0.5f, -0.5f, // 1 bottom back right
         0.5f,  0.5f, -0.5f, // 2 top back right
        -0.5f,  0.5f, -0.5f, // 3 top back left
        -0.5f, -0.5f,  0.5f, // 4 front bottom left
         0.5f, -0.5f,  0.5f, // 5 front bottom right
         0.5f,  0.5f,  0.5f, // 6 front top right
        -0.5f,  0.5f,  0.5f  // 7 front top left
    };
    
    unsigned int indices[] = {
        0,1,2, 2,3,0, // back
        4,5,6, 6,7,4, // front
        0,4,7, 7,3,0, // left
        1,5,6, 6,2,1, // right
        0,1,5, 5,4,0, // bottom
        3,2,6, 6,7,3  // top
    };
    MeshBuilder *builder = init_builder(cubeVerts, indices, 8, 36);
    Mesh *with_normals = generate_mesh(builder);
    


    GLFWwindow* window = setupwindow();
    if (!window) return -1;
    
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, with_normals->vertex_count * 6 * sizeof(float), with_normals->vertices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    

    Shader *vertshader = init_shader(GL_VERTEX_SHADER, "assets/default_vert.vert");
    compile_shader(vertshader);
    Shader *fragshader = init_shader(GL_FRAGMENT_SHADER, "assets/default_frag.frag");
    compile_shader(fragshader);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertshader->shaderptr);
    glAttachShader(shaderProgram, fragshader->shaderptr);
    glLinkProgram(shaderProgram);
    free_shader(vertshader);
    free_shader(fragshader);

    mat4 projection;
    glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);

    glEnable(GL_DEPTH_TEST);

    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        process_input(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 view;
        vec3 center;
        glm_vec3_add(cameraPos, cameraFront, center);
        glm_lookat(cameraPos, center, cameraUp, view);

        glUseProgram(shaderProgram);
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float *)projection);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
