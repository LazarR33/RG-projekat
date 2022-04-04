#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//changes

struct PointLight{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight{
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

//


int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PET SIMS", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



   //face culling

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);


    //shaders
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader dogShader("resources/shaders/light.vs", "resources/shaders/light.fs");
    Shader framebuffersShader("resources/shaders/framebuffers.vs","resources/shaders/framebuffers.fs");
    Shader texShader("resources/shaders/texture.vs", "resources/shaders/texture.fs");
    Shader statueShader("resources/shaders/light.vs", "resources/shaders/light.fs");

    //lights
    PointLight pointLight;
    pointLight.position = glm::vec3 (4.0f, 4.0f, 0.0f);
    pointLight.ambient = glm::vec3 (1.0f, 0.6f, 0.2f);
    pointLight.diffuse = glm::vec3 (0.9f, 0.5f, 0.6f);
    pointLight.specular = glm::vec3 (1.0f, 1.0f, 1.0f);

    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;

    DirLight dirLight;
    dirLight.direction = glm::vec3 (0.0f, -1.0f, -1.0f);
    dirLight.ambient = glm::vec3 (0.1f, 0.1f, 0.1f);
    dirLight.diffuse = glm::vec3 (0.5f, 0.5f, 0.5f);
    dirLight.specular = glm::vec3 (0.5f, 0.5f, 0.5f);

    //
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
    float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
    };

     float stoneVertices[]={// top
        1.0f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,      //A0
            1.0f, 0.1f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,     //B1
            -1.0f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,     //C2

            1.0f, 0.1f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,     //B1
            -1.0f, 0.1f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,    //D3
            -1.0f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,     //C2

            // right
            1.0f, 0.1f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.2f,     //B1
            1.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.2f,      //A0
            1.0f, -0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     //E4

            1.0f, 0.1f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.2f,     //B1
            1.0f, -0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     //E4
            1.0f, -0.1f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    //F5

            // back
            -1.0f, 0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.2f,    //D3
            1.0f, 0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.2f,     //B1
            1.0f, -0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,    //F5

            -1.0f, 0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.2f,    //D3
            1.0f, -0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,    //F5
            -1.0f, -0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   //H7

            // left
            -1.0f, 0.1f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.2f,    //C2
            -1.0f, 0.1f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.2f,   //D3
            -1.0f, -0.1f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   //G6

            -1.0f, -0.1f, 1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  //G6
            -1.0f, 0.1f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.2f,   //D3
            -1.0f, -0.1f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  //H7

            // front
            1.0f, 0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.2f,     //A0
            -1.0f, 0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.2f,    //C2
            1.0f, -0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,    //E4

            1.0f, -0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,    //E4
            -1.0f, 0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.2f,    //C2
            -1.0f, -0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,   //G6

            // bottom
            1.0f, -0.1f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   //F5
            1.0f, -0.1f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,    //E4
            -1.0f, -0.1f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,   //G6

            1.0f, -0.1f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   //F5
            -1.0f, -0.1f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,   //G6
            -1.0f, -0.1f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  //H7
};


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(stoneVertices), stoneVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;

    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/wood3.jpg").c_str(), &width, &height, &nrChannels, 0);
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







    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


    framebuffersShader.use();
    framebuffersShader.setInt("screenTexture",0);


    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/cube/right.jpg"),
                    FileSystem::getPath("resources/textures/cube/left.jpg"),
                    FileSystem::getPath("resources/textures/cube/top.jpg"),
                    FileSystem::getPath("resources/textures/cube/bottom.jpg"),
                    FileSystem::getPath("resources/textures/cube/front.jpg"),
                    FileSystem::getPath("resources/textures/cube/back.jpg")
            };
    unsigned int cubemapTexture = loadCubemap(faces);



    //model loading
    Model dogModel("resources/objects/dog/source/dog.fbx");
    Model statueModel("resources/objects/wooden-statue-of-the-owl/source/drevena_sova_ratibor/drevena_sova_ratibor.FBX");




    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
        glEnable(GL_DEPTH_TEST);


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D,texture);



        texShader.use();

        glm::mat4 model1 = glm::mat4(1.0f);
        glm::mat4 view1 = camera.GetViewMatrix();
        glm::mat4 projection1 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.1f, 100.0f);

        texShader.setMat4("projection", projection1);
        texShader.setMat4("view",view1);



        model1=glm::translate(model1,glm::vec3(0.95f, -0.5f, 0.1f));
        model1=glm::rotate(model1, glm::radians(10.0f), glm::vec3(1.0f, 0.2f, 0.3f));
        model1 = glm::scale(model1, glm::vec3(0.45f));
        texShader.setMat4("model", model1);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);









        dogShader.use();


        dogShader.setVec3("pointLight.position", pointLight.position);
        dogShader.setVec3("pointLight.ambient", pointLight.ambient);
        dogShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        dogShader.setVec3("pointLight.specular", pointLight.specular);

        dogShader.setFloat("pointLight.constant", pointLight.constant);
        dogShader.setFloat("pointLight.linear", pointLight.linear);
        dogShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        dogShader.setVec3("viewPosition", camera.Position);


        dogShader.setVec3("dirLight.direction", dirLight.direction);
        dogShader.setVec3("dirLight.ambient", dirLight.ambient);
        dogShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        dogShader.setVec3("dirLight.specular", dirLight.specular);
        //




        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.1f, 100.0f);

        model = glm::translate(model, glm::vec3(-0.25f,-1.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.01f));
        model=glm::rotate(model,glm::radians(60.0f),glm::vec3(0.0f,-1.0f,0.0f));



        dogShader.setMat4("projection", projection);
        dogShader.setMat4("view", view);

        dogShader.setMat4("model", model);
        dogModel.Draw(dogShader);


        //CHANGE
        statueShader.use();


        statueShader.setVec3("pointLight.position", pointLight.position);
        statueShader.setVec3("pointLight.ambient", pointLight.ambient);
        statueShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        statueShader.setVec3("pointLight.specular", pointLight.specular);

        statueShader.setFloat("pointLight.constant", pointLight.constant);
        statueShader.setFloat("pointLight.linear", pointLight.linear);
        statueShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        statueShader.setVec3("viewPosition", camera.Position);


        statueShader.setVec3("dirLight.direction", dirLight.direction);
        statueShader.setVec3("dirLight.ambient", dirLight.ambient);
        statueShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        statueShader.setVec3("dirLight.specular", dirLight.specular);





        glm::mat4 model2 = glm::mat4(1.0f);
        glm::mat4 view2 = camera.GetViewMatrix();
        glm::mat4 projection2 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.1f, 100.0f);
        //projection2 = glm::scale(projection2, glm::vec3(1.0f, -1.0f, 1.0f));
        model2 = glm::translate(model2, glm::vec3(0.8f, 0.2f, 0.5f));
        model2 = glm::scale(model2, glm::vec3(0.07f));
        model2=glm::rotate(model2,glm::radians(260.0f),glm::vec3(0.0f,-1.0f,0.0f));
        model2=glm::rotate(model2,glm::radians(180.0f),glm::vec3(0.0f,0.0f,1.0f));

        statueShader.setMat4("projection", projection2);
        statueShader.setMat4("view", view2);


        statueShader.setMat4("model", model2);


        statueModel.Draw(statueShader);

        //

        // draw skybox as last
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        skyboxShader.setInt("skybox", 0);

        skyboxShader.setMat4("view", glm::mat4 (glm::mat3(view)));
        skyboxShader.setMat4("projection", projection);



        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);


        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);


        framebuffersShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D,textureColorbuffer);
        glDrawArrays(GL_TRIANGLES,0,6);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }




    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float speed = deltaTime * 0.5f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, speed);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, speed);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0, 30.0);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0, -30.0);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessMouseMovement(30.0, 0.0);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessMouseMovement(-30.0, 0.0);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {

    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
   camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

}
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed" << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


    