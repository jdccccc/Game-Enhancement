#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 将顶点位置变换到世界空间，并传递给片段着色器
    FragPos = vec3(model * vec4(aPos, 1.0));
    // 将法线向量变换到世界空间（使用法线矩阵），并传递给片段着色器
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // 将顶点位置变换到裁剪空间
    gl_Position = projection * view * vec4(FragPos, 1.0);
}