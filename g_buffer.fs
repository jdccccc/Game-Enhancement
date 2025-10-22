#version 330 core
// 定义 G-Buffer 的输出。每个 'out' 变量对应 FBO 的一个颜色附件。
// layout (location = N) 指定了这个输出变量写入到 FBO 的第 N 个颜色附件。
layout (location = 0) out vec3 gPosition;  // 输出附件0: 片段的世界空间位置
layout (location = 1) out vec3 gNormal;    // 输出附件1: 片段的世界空间法线
layout (location = 2) out vec4 gAlbedoSpec;// 输出附件2: 片段的反照率颜色(rgb)和镜面强度(a)

// 从顶点着色器接收插值后的数据 (与 basic_lighting.vs 的输出一致)
in vec3 FragPos;      // 接收顶点着色器传来的世界空间位置
in vec3 Normal;       // 接收顶点着色器传来的世界空间法线
in vec2 TexCoords;    // 接收顶点着色器传来的纹理坐标

// 纹理采样器 (用于获取物体的反照率颜色)
uniform sampler2D texture_diffuse1; // 对应之前 basic_lighting.fs 中的 texture1

void main()
{
    // 将插值后的世界空间位置直接写入 gPosition 附件
    gPosition = FragPos; //

    // 将插值后的法线进行标准化 (确保长度为 1)，然后写入 gNormal 附件
    // 标准化对于后续在光照阶段进行正确的光照计算至关重要
    gNormal = normalize(Normal); //

    // 从漫反射纹理中采样该片段的反照率颜色 (Albedo Color)
    // .rgb 表示我们只取纹理颜色值的 R, G, B 分量
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb; //

    // 将镜面反射强度存储在 gAlbedoSpec 的 alpha 通道中
    // 这里我们暂时使用一个固定的值 0.5 作为示例
    gAlbedoSpec.a = 0.5f;
}