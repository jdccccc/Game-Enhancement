#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp> // 包含GLM库的头文件,用于处理向量和矩阵运算
#include <glm/gtc/matrix_transform.hpp> // 包含GLM库的矩阵变换头文件,用于计算视图矩阵

#include <vector>

// 定义摄像机移动的几个可能选项。
// 用作抽象，以避免与特定于窗口系统的输入方法相关联。
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// 默认摄像机值
// 坐标单位无量纲，时间单位为秒通过glfwGetTime()获取，旋转角度单位为度

// YAW,绕Y轴旋转角度，初始值为-90.0f，指向负Z轴方向。
const float YAW         = -90.0f;
// PITCH,绕X轴旋转角度，初始值为0.0f。
const float PITCH       =  0.0f;

// 摄像机移动的速度，在ProcessKeyboard中使用,这个值与deltaTime相乘,以获得实际移动距离。
const float SPEED       =  2.5f;
// 鼠标灵敏度，控制摄像机视角旋转的响应速度，在ProcessMouseMove方法中，鼠标便宜量乘以这个值。
const float SENSITIVITY =  0.1f;
// 摄像机的视野角度
const float ZOOM        =  45.0f;


// 一个抽象的摄像机类，用于处理输入并计算相应的欧拉角、向量和矩阵，以便在OpenGL中使用
class Camera
{
public:
    // 摄像机属性
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // 欧拉角
    float Yaw;
    float Pitch;
    // 摄像机选项
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // 使用向量构造函数
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // 使用标量值构造函数
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // 返回使用欧拉角和LookAt矩阵计算的视图矩阵
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // 处理从任何类似键盘的输入系统接收到的输入。
    // 接受摄像机定义的ENUM形式的输入参数（以将其与窗口系统分离）
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // 处理从鼠标输入系统接收到的输入。
    // 需要x和y方向上的偏移值。
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // 确保当俯仰角超出边界时，屏幕不会翻转
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // 从更新的欧拉角更新Front、Right和Up向量
        updateCameraVectors();
    }

    // 处理从鼠标滚轮事件接收到的输入。
    // 仅需要在垂直滚轮轴上输入
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // 从摄像机的（更新的）欧拉角计算前向量
    void updateCameraVectors()
    {
        // 计算新的Front向量
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // 同时重新计算Right和Up向量
        Right = glm::normalize(glm::cross(Front, WorldUp));  // 标准化向量，因为向上或向下看时它们的长度会接近0，这会导致移动变慢。
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif