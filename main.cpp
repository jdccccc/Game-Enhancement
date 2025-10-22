#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem> // C++17 标准库中的文件系统库, 用于文件路径操作

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // 包含 glm::value_ptr, 用于将 glm 类型转换为指针, 方便 OpenGL 函数调用

// 假设Shader类和Camera类在您的包含路径中
#include "shader_m.h" // 您需要从LearnOpenGL或您自己的实现中获取此类
#include "camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// 设置
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 摄像机
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 时间
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// 光源
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    // glfw: 初始化和配置
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw 窗口创建
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 隐藏鼠标光标并将其捕获到窗口中
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: 加载所有OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 配置全局opengl状态
    // 启用深度测试
    // 深度测试用于确定哪些像素应该被绘制，哪些应该被遮挡
    // 启用深度测试后，OpenGL会根据每个像素的深度值来判断是否绘制该像素
    glEnable(GL_DEPTH_TEST);

    // 构建和编译我们的着色器程序
    // 注意：您需要提供一个可以从文件加载着色器的Shader类
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    
    // 使用相对路径指向上一级目录中的着色器文件
    Shader cubeShader("../basic_lighting.vs", "../basic_lighting.fs");
    if (cubeShader.ID == 0) {
        std::cout << "ERROR: Failed to load lighting shader!" << std::endl;
        return -1;
    }
    
    Shader lightCubeShader("../light_cube.vs", "../light_cube.fs"); // 为光源立方体提供单独的着色器
    if (lightCubeShader.ID == 0) {
        std::cout << "ERROR: Failed to load light cube shader!" << std::endl;
        return -1;
    }

    // 设置顶点数据（和缓冲区）并配置顶点属性
    // 立方体的顶点数据
    // 一共六面，每个面2个三角形，每个三角形3个顶点
    // 每个顶点两个属性：位置和法向量
    // 法向量决定了光线如何从物体表面反射
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    
    // 首先，配置立方体的VAO（和VBO）
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 其次，配置光源的VAO（VBO保持不变；光源对象也是一个3D立方体，顶点相同）
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    // 只需要位置数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 局部坐标采用无量纲单位，没有固定物理尺寸，坐标值表示的是模型内部的相对比例关系

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 每帧的时间逻辑
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 输入
        processInput(window);

        // 渲染
        // 设置清除颜色为深灰色
        // 这个颜色会在每次调用glClear时填充颜色缓冲区，作为背景颜色
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        // 清除颜色缓冲区和深度缓冲区
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 在设置uniforms/绘制对象时确保激活着色器
        cubeShader.use();
        // 设置物体的颜色为橙色调 (RGB: 1.0, 0.5, 0.31)
        cubeShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        
        // 设置光源的颜色为白色 (RGB: 1.0, 1.0, 1.0)
        cubeShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        
        // 设置光源在世界空间中的位置
        cubeShader.setVec3("lightPos", lightPos);
        
        // 设置观察者（相机）在世界空间中的位置，用于光照计算
        cubeShader.setVec3("viewPos", camera.Position);

        // 视图/投影变换
        // 创建投影矩阵：将3D场景投影到2D屏幕上
        // glm::perspective: 创建透视投影矩阵，模拟人眼的透视效果
        //   - glm::radians(camera.Zoom): 将相机的视野角度从度转换为弧度
        //   - (float)SCR_WIDTH / (float)SCR_HEIGHT: 屏幕宽高比，用于正确显示图像
        //   - 0.1f: 近裁剪平面，距离相机最近的可见点
        //   - 100.0f: 远裁剪平面，距离相机最远的可见点
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        // 创建视图矩阵：定义相机的位置和朝向
        // camera.GetViewMatrix(): 从相机对象获取视图矩阵，包含相机的位置、目标点和上向量
        glm::mat4 view = camera.GetViewMatrix();
        
        // 将投影矩阵传递给着色器
        // cubeShader.setMat4: 将4x4矩阵传递给着色器中的uniform变量
        //   - "projection": 着色器中uniform变量的名称
        //   - projection: 要传递的投影矩阵
        cubeShader.setMat4("projection", projection);
        
        // 将视图矩阵传递给着色器
        // cubeShader.setMat4: 将4x4矩阵传递给着色器中的uniform变量
        //   - "view": 着色器中uniform变量的名称
        //   - view: 要传递的视图矩阵
        cubeShader.setMat4("view", view);

        // 世界变换
        // 创建模型矩阵：定义物体在世界空间中的位置、旋转和缩放
        // glm::mat4(1.0f): 创建一个4x4单位矩阵，表示无变换的初始状态
        glm::mat4 model = glm::mat4(1.0f);
        
        // 将模型矩阵传递给着色器
        // cubeShader.setMat4: 将4x4矩阵传递给着色器中的uniform变量
        //   - "model": 着色器中uniform变量的名称
        //   - model: 要传递的模型矩阵（这里是单位矩阵，表示物体位于世界坐标系原点）
        cubeShader.setMat4("model", model);

        // 渲染立方体
        // 绑定立方体的顶点数组对象
        // glBindVertexArray: 指定要使用的顶点数组对象，包含顶点属性配置
        glBindVertexArray(cubeVAO);
        
        // 绘制立方体
        // glDrawArrays: 使用当前绑定的顶点数组对象绘制图元
        //   - GL_TRIANGLES: 指定要绘制的图元类型为三角形
        //   - 0: 从顶点数组的起始位置开始绘制
        //   - 36: 要绘制的顶点数量（立方体有6个面，每个面2个三角形，每个三角形3个顶点，共36个顶点）
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 同时绘制光源对象
        // 切换到光源着色器程序
        // lightCubeShader.use(): 激活光源着色器程序，后续渲染将使用此着色器
        lightCubeShader.use();
        
        // 将投影矩阵传递给光源着色器
        // lightCubeShader.setMat4: 将4x4矩阵传递给光源着色器中的uniform变量
        //   - "projection": 着色器中uniform变量的名称
        //   - projection: 要传递的投影矩阵（与主场景相同）
        lightCubeShader.setMat4("projection", projection);
        
        // 将视图矩阵传递给光源着色器
        // lightCubeShader.setMat4: 将4x4矩阵传递给光源着色器中的uniform变量
        //   - "view": 着色器中uniform变量的名称
        //   - view: 要传递的视图矩阵（与主场景相同）
        lightCubeShader.setMat4("view", view);
        
        // 重置模型矩阵为单位矩阵
        model = glm::mat4(1.0f);
        
        // 将光源立方体平移到光源位置
        // glm::translate: 创建平移矩阵，将物体沿指定向量移动
        //   - model: 原始矩阵
        //   - lightPos: 平移向量，即光源在世界空间中的位置
        model = glm::translate(model, lightPos);
        
        // 缩小光源立方体
        // glm::scale: 创建缩放矩阵，改变物体的大小
        //   - model: 原始矩阵
        //   - glm::vec3(0.2f): 缩放因子，在所有三个轴上缩小到原来的0.2倍
        model = glm::scale(model, glm::vec3(0.2f)); // 一个更小的立方体
        
        // 将变换后的模型矩阵传递给光源着色器
        // lightCubeShader.setMat4: 将4x4矩阵传递给光源着色器中的uniform变量
        //   - "model": 着色器中uniform变量的名称
        //   - model: 要传递的模型矩阵（包含平移和缩放变换）
        lightCubeShader.setMat4("model", model);

        // 绑定光源立方体的顶点数组对象
        // glBindVertexArray: 指定要使用的顶点数组对象，包含顶点属性配置
        glBindVertexArray(lightCubeVAO);
        
        // 绘制光源立方体
        // glDrawArrays: 使用当前绑定的顶点数组对象绘制图元
        //   - GL_TRIANGLES: 指定要绘制的图元类型为三角形
        //   - 0: 从顶点数组的起始位置开始绘制
        //   - 36: 要绘制的顶点数量（立方体有6个面，每个面2个三角形，每个三角形3个顶点，共36个顶点）
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: 交换缓冲区并轮询IO事件（按下的键、鼠标移动等）
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 可选：释放所有资源
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: 终止，清除所有先前分配的GLFW资源。
    glfwTerminate();
    return 0;
}

// 处理所有输入：查询GLFW是否在此帧中按下了相关键并做出相应反应
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: 每当窗口大小改变时，此回调函数执行
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // TODO: 说明如何将渲染结果转换为屏幕上的可见图像
    glViewport(0, 0, width, height);
}

// glfw: 每当鼠标移动时，此回调函数被调用
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    // 鼠标坐标的原点位于窗口的左上角
    // 单位：像素, 每个单位表示屏幕上的一个物理像素点
    // X坐标范围 [0, 窗口宽度)
    // Y坐标范围 [0, 窗口高度)
    // X坐标从左到右增加
    // Y坐标从上到下增加
    float xpos = static_cast<float>(xposIn);
    // static_cast 用于类型转换，在编译时进行检查，只允许合理的类型转换
    // 不能用于无关类型之间的转换，比如将指针转换为整数类型，或者去除指针的const属性
    // double 可以更准确地表示鼠标位置
    // 然而，在OpenGL和GLM库中，许多函数都期望接收浮点数类型的参数
    // 因此，需要将double类型的鼠标位置转换为float类型
    float ypos = static_cast<float>(yposIn);

    // 初始化鼠标位置
    if (firstMouse)
    {
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

// glfw: 每当鼠标滚轮滚动时，此回调函数被调用
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // 鼠标滚轮滚动时，yoffset 为正值表示向上滚动，负值表示向下滚动
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}