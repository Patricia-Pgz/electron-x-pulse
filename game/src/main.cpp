#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
const char *vertexShaderSource = R"(
    #version 460 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 mvp;
    void main() {
        gl_Position = mvp * vec4(aPos.xyz, 1.0);
    }
)";
const char *fragmentShaderSource = R"(
    #version 460 core
    out vec4 fragColor;
    void main() {
        fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
)";

float vertices[] = {
    0.5f, 0.025f, 0.0f,
    0.0f, 0.3f, 0.0f,
    -0.2f, 0.05f, 0.0f,

    0.5f, -0.025f, 0.0f,
    0.0f, -0.3f, 0.0f,
    -0.2f, -0.05f, 0.0f
};

float zRotation = 0.0f;
float rotationSpeed = 120.0f;
float xTranslate = 0.0f;
float yTranslate = 0.0f;
float translationSpeed = 1.0f;
float lastFrameTime = 0.0f;
float deltaTime = 0.0f;

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        zRotation += rotationSpeed * deltaTime;
    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        zRotation -= rotationSpeed * deltaTime;
    }

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        yTranslate += sin(glm::radians(zRotation)) * translationSpeed * deltaTime;
        xTranslate += cos(glm::radians(zRotation)) * translationSpeed * deltaTime;
    }

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        yTranslate -= sin(glm::radians(zRotation)) * translationSpeed * deltaTime;
        xTranslate -= cos(glm::radians(zRotation)) * translationSpeed * deltaTime;
    }
}

glm::mat4 calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position));
    model = glm::scale(model, scale);
    model = glm::rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 90.0f),
                            glm::vec3(0.0f, 0.0f, 0.0),
                            glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 projection = glm::perspective(glm::radians(2.0f), 1000.0f/600.0f, 0.1f, 100.0f);
    return projection * view * model;
}

int main()
{
    glfwInit();
    // Define OpenGL version (4.6)
    // -------------------------------------------------
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(800, 600, "GameLabIII", nullptr, nullptr);
    if(window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    glDetachShader(shaderProgram, vertexShader); //TODO: wieso hier schon detach?
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) nullptr);
    glEnableVertexAttribArray(0);

    glfwSetTime(1.0/60);

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.172f, 0.243f, 0.313f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window);

        // Calculate MVP matrix
        glm::mat4 mvpMatrix = calculateMvpMatrix(glm::vec3(xTranslate, yTranslate, 0), zRotation, glm::vec3(.25f, .25f, .25f));

        // Pass MVP matrix to shader
        int mvpLocation = glGetUniformLocation(shaderProgram, "mvp");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Update delta time
        float frameTime = glfwGetTime();
        deltaTime = frameTime - lastFrameTime;
        lastFrameTime = frameTime;
        std::cout << "Delta Time: " << deltaTime << "s" << std::endl;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO); //TODO: nachschauen warum &
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
