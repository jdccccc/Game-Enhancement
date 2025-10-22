// 文件名: lighting_pass.fs
#version 330 core
out vec4 FragColor; // 输出最终的颜色到屏幕

in vec2 TexCoords; // 从顶点着色器接收的纹理坐标 (屏幕空间)

// G-Buffer 纹理采样器
uniform sampler2D gPosition;    // 位置纹理
uniform sampler2D gNormal;      // 法线纹理
uniform sampler2D gAlbedoSpec;  // 反照率(rgb) + 镜面强度(a) 纹理

// 光照所需的 Uniform 变量
uniform vec3 lightPos;   // 光源位置 (世界空间)
uniform vec3 viewPos;    // 观察者/相机位置 (世界空间)
uniform vec3 lightColor; // 光源颜色

void main()
{
    // 1. 从 G-Buffer 恢复几何/颜色信息
    // 使用传入的屏幕空间纹理坐标 TexCoords 对 G-Buffer 纹理进行采样
    vec3 FragPos = texture(gPosition, TexCoords).rgb;     // 获取世界空间位置
    vec3 Normal = texture(gNormal, TexCoords).rgb;        // 获取世界空间法线
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;    // 获取反照率颜色
    float SpecularStrength = texture(gAlbedoSpec, TexCoords).a; // 获取镜面强度

    // 2. 执行光照计算 (这里使用和之前前向渲染类似的 Blinn-Phong 模型)
    vec3 lighting = vec3(0.0); // 初始化光照结果

    // 检查法线是否有效（可选，避免在背景像素上计算光照）
    // 如果法线长度接近 0，则认为这是背景，直接输出黑色并返回
    if(length(Normal) > 0.1) // 避免对背景像素进行光照计算 (法线通常为 0)
    {
         // 环境光 (Ambient)
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;

        // 漫反射光 (Diffuse)
        vec3 lightDir = normalize(lightPos - FragPos); // 光线方向
        float diff = max(dot(Normal, lightDir), 0.0); // 漫反射强度因子
        vec3 diffuse = diff * lightColor;

        // 镜面反射光 (Specular - Blinn-Phong)
        vec3 viewDir = normalize(viewPos - FragPos);      // 观察方向
        vec3 halfwayDir = normalize(lightDir + viewDir); // 半程向量
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0); // 镜面反射强度因子 (32 是高光指数)
        vec3 specular = SpecularStrength * spec * lightColor; // 乘以从 G-Buffer 获取的镜面强度

        // 最终光照 = (环境光 + 漫反射光 + 镜面反射光) * 物体反照率颜色
        lighting = (ambient + diffuse + specular) * Albedo;
    }
    else
    {
        lighting = Albedo; // 对于背景像素，直接使用 Albedo（可能只是清除色）
    }


    // 输出最终计算出的颜色
    FragColor = vec4(lighting, 1.0);
}