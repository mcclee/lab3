#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>
#include <gtx/euler_angles.hpp>
#include "ShaderLoader.h"
#include "Obj_Loader.hpp"
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


using namespace std;

void bindall(GLuint VBO, GLuint VAO, GLuint EBO, vector < glm::vec3 > vertices, vector < unsigned int > indices);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void bind_texture(unsigned int texture, const char* path);
void processInput(GLFWwindow *window);
glm::mat4 generateRotate(glm::vec4 orient);
void mouse(GLFWwindow *window, double xpos, double ypos);

bool OrgMouse = true;
//const char* wallpath = '';
// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;
const char* ball[3] = {"C:\\Users\\40994\\Downloads\\mt\\soccer_ball.obj", "C:\\Users\\40994\\CLionProjects\\test\\source\\ball.vs", "C:\\Users\\40994\\CLionProjects\\test\\source\\ball.fs"};
const char* wall[3] = {"C:\\Users\\40994\\Downloads\\mt\\OBJ\\wall.obj", "C:\\Users\\40994\\CLionProjects\\test\\source\\wall.vs", "C:\\Users\\40994\\CLionProjects\\test\\source\\wall.fs"};
float yaw = -90.0f;    // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
const char* texture_path = "C:\\Users\\40994\\Downloads\\mt\\Textures\\soccer_ball_textures\\soccer_ball_diffuse.bmp";
GLfloat B_splinesArray[16] =
        {
                -1.0 / 6,    3.0 / 6,    -3.0 / 6,   1.0 / 6,
                3.0 / 6,    -6.0 / 6,         0.0,   4.0 / 6,
                -3.0 / 6,    3.0 / 6,    3.0 / 6,    1.0 / 6,
                1.0 / 6,         0.0,        0.0,        0.0
        };

float catmullRomArray[16] =
        {
                -0.5,   1,   -0.5,  0,
                1.5,    -2.5,   0,  1,
                -1.5,   2,  0.5,    0,
                0.5,    -0.5,   0,  0
        };

float lX = SCR_WIDTH / 2.0;
float lY = SCR_HEIGHT / 2.0;
glm::vec3 Cpos = glm::vec3(0.0f, 100.0f, 2000.0f);
glm::vec3 CFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 CUp = glm::vec3(0.0f, 1.0f, 0.0f);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    vector< glm::vec3 > wall_vertices;
    vector<unsigned int> wall_indices;
    vector< glm::vec3 > ball_vertices;
    vector< unsigned int > ball_indices;

    //splines
    glm::mat4 B_SplinesM = glm::make_mat4(B_splinesArray);
    glm::mat4 CatmullRomM = glm::make_mat4(catmullRomArray);
    glm::mat4 M;

    float tt = 0.1;
    glm::vec4 TT(tt*tt*tt, tt*tt, tt, 1);
    glm::vec4 CC = TT*B_SplinesM;

    //shader Load
    Shader wall_shader(wall[1], wall[2]);
    Shader ball_shader(wall[1], wall[2]);

    Obj_Loader::load_obj(wall[0], wall_vertices, wall_indices);
    Obj_Loader::load_obj(ball[0], ball_vertices, ball_indices);

    unsigned int wall_VBO, wall_VAO, wall_EBO;
    unsigned int ball_VBO, ball_VAO, ball_EBO;
    unsigned int texture;

    bindall(wall_VBO, wall_VAO, wall_EBO, wall_vertices, wall_indices);
    bindall(ball_VBO, ball_VAO, ball_EBO, ball_vertices, ball_indices);

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 3000.0f);

    wall_shader.use();
    wall_shader.setMat4("projection", projection);
    ball_shader.use();
    ball_shader.setMat4("projection", projection);
    bind_texture(texture, texture_path);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    float timeSlowRate = 1;
    float lastKeyEndingTime = 0;
    glm::mat4 interOrientation;
    glm::mat4 interPosition;

    float oneleg = 200.0f;
    int step_length = 0;
    int cycles = 0;
    // rotate quaternions
    glm::vec4 smallrot = glm::vec4(0.0, -0.703, 0.0, 0.713);
    glm::mat4 rotation = generateRotate(smallrot);
    glm::mat4 walks = generateRotate(glm::vec4(0.0, 0.00211, 0.0, 0.99999999));
    glm::vec4 Qrotation = glm::vec4(0.707, 0.0, 0.0, 0.707);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        glClearColor(0.67578125f, 0.84375f, 0.8984375f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, texture);
        auto currentTime = float(glfwGetTime());
        float used_time = (currentTime - lastKeyEndingTime) / timeSlowRate;

        if (cycles != 0){
            rotation = rotation * walks;
        }
        cycles ++;
        glm::mat4 model_wall = glm::mat4(1.0);
        glm::mat4 model_ball = glm::mat4(1.0);
        glm::mat4 view = glm::mat4(1.0);
        view = glm::lookAt(Cpos, Cpos + CFront, CUp);
        wall_shader.use();
        wall_shader.setMat4("view", view);
        ball_shader.use();
        ball_shader.setMat4("view", view);
        wall_shader.use();
        wall_shader.setMat4("model", model_wall);
        /*
         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(wall_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, wall_VBO);
        glDrawElements(GL_TRIANGLES, wall_indices.size(), GL_UNSIGNED_INT, nullptr);
         */
        ball_shader.use();
        ball_shader.setMat4("model", rotation * model_ball);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(ball_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, ball_VBO);
        glDrawElements(GL_TRIANGLES, ball_indices.size(), GL_UNSIGNED_INT, nullptr);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void bindall(GLuint VBO, GLuint VAO, GLuint EBO, vector < glm::vec3 > vertices, vector < unsigned int > indices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*) nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

glm::mat4 generateRotate(glm::vec4 orient) {
    float x = orient.x;
    float y = orient.y;
    float z = orient.z;
    float w = orient.w;
    glm::mat3 rotation(1.0f);
    rotation[0][0] = 1 - 2 * y*y - 2 * z*z;
    rotation[0][1] = 2 * x*y - 2 * w*z;
    rotation[0][2] = 2 * x*z + 2 * w*y;
    rotation[1][0] = 2 * x*y + 2 * w*z;
    rotation[1][1] = 1 - 2 * x*x - 2 * z*z;
    rotation[1][2] = 2 * y*z - 2 * w*x;
    rotation[2][0] = 2 * x*z - 2 * w*y;
    rotation[2][1] = 2 * y*z + 2 * w*x;
    rotation[2][2] = 1 - 2 * x*x - 2 * y*y;
    return glm::transpose(glm::mat4(rotation));
}

void bind_texture(unsigned int texture, const char* path){
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void mouse(GLFWwindow *window, double xpos, double ypos)
{
    if (OrgMouse)
    {
        lX = xpos;
        lY = ypos;
        OrgMouse = false;
    }

    float xoffset = xpos - lX;
    float yoffset = lY - ypos;
    lX = xpos;
    lY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    CFront = glm::normalize(front);
}