#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

int main()
{


    if( !glfwInit() )
    {
        std::cout << "Failed to init glfw\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    GLFWwindow* window;
    window = glfwCreateWindow( 1024, 720, "RayMarcher", NULL, NULL);
    if (!window){
        std::cout << "Failed to open window\n";
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 1024, 720);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    std::string line;
    std::ifstream vert("../vert.glsl");
    std::stringstream buffer;
    while(getline(vert, line))
    {
        buffer<<line<<std::endl;
    }
    vert.close();
    std::string vertCode = buffer.str();

    std::stringstream buffer1;
    std::ifstream frag("../frag.glsl");
    while(getline(frag, line))
    {
        buffer1<<line<<std::endl;
    }
    frag.close();
    std::string fragCode = buffer1.str();



    const char *vertexShaderSource = vertCode.c_str();

    const char* fragmentShaderSource = fragCode.c_str();

    float vertices[] = {
            1.0f,  1.0f, 0.0f,  // top right
            1.0f, -1.0f, 0.0f,  // bottom right
            -1.0f, -1.0f, 0.0f,  // bottom left
            -1.0f,  1.0f, 0.0f   // top left
    };
    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
    };


    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);





    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);


    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    float camPos[3] = { 0.0f, 0.0f, 3.0f};
    float greenBallPosition[3] = {0.0f,0.0f,-3.0f};
    float redBallPosition[3] = {0.0f,2.0f,-3.0f};
    float blendingDistance = 0.5f;

    glUseProgram(shaderProgram);

    int iResolutionLocation = glGetUniformLocation(shaderProgram, "iResolution");
    glUniform2f(iResolutionLocation,1024,720);

    int camPosLocation = glGetUniformLocation(shaderProgram,"camPos");
    glUniform3f(camPosLocation,camPos[0],camPos[1],camPos[2]);

    int redBallPosLocation = glGetUniformLocation(shaderProgram,"redBallPos");
    glUniform3f(redBallPosLocation,redBallPosition[0],redBallPosition[1],redBallPosition[2]);

    int greenBallPosLocation = glGetUniformLocation(shaderProgram,"greenBallPos");
    glUniform3f(greenBallPosLocation,greenBallPosition[0],greenBallPosition[1],greenBallPosition[2]);

    int blendingDistanceLocation = glGetUniformLocation(shaderProgram,"blendingDistance");
    glUniform1f(blendingDistanceLocation,blendingDistance);


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    while (!glfwWindowShouldClose(window)){
        //ImGUIStuff
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::ShowDemoWindow(); // Show demo window! :)


        ImGui::DragFloat3("Camera",camPos,0.1f,-50.0f,50.0f);
        glUniform3f(camPosLocation,camPos[0],camPos[1],camPos[2]);

        ImGui::DragFloat3("Red Pos",redBallPosition,0.1f,-50.0f,50.0f);
        glUniform3f(redBallPosLocation,redBallPosition[0],redBallPosition[1],redBallPosition[2]);

        ImGui::DragFloat3("Green Pos",greenBallPosition,0.1f,-50.0f,50.0f);
        glUniform3f(greenBallPosLocation,greenBallPosition[0],greenBallPosition[1],greenBallPosition[2]);

        ImGui::DragFloat("BlendingDistance",&blendingDistance,0.1f,-50.f,50.0f);
        glUniform1f(blendingDistanceLocation,blendingDistance);


        //My rendering Stuff
        glClear( GL_COLOR_BUFFER_BIT );

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}



