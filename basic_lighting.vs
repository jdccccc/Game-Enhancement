#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords; // 接收纹理坐标属性

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords; // 将纹理坐标传递给片段着色器

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords; // 传递纹理坐标
    gl_Position = projection * view * vec4(FragPos, 1.0); //
}