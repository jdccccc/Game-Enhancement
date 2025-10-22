#version 330 core
layout (location = 0) in vec3 aPos;     // 顶点位置 (屏幕空间四边形的顶点)
layout (location = 1) in vec2 aTexCoords; // 顶点纹理坐标 (用于采样 G-Buffer)

out vec2 TexCoords; // 输出给片段着色器的纹理坐标

void main()
{
    // 直接传递纹理坐标
    TexCoords = aTexCoords;
    // 输出顶点位置。由于我们渲染的是一个覆盖整个裁剪空间的四边形，
    // 顶点位置已经在标准化设备坐标 (NDC) 中，可以直接赋给 gl_Position。
    // z=0.0 确保它在近裁剪平面上。
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}