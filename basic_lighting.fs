#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords; // 接收顶点着色器传来的纹理坐标

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
// uniform vec3 objectColor; // 不再需要固定的物体颜色
uniform sampler2D texture1; // 纹理采样器

void main()
{
    // 环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor; //

    // 漫反射光
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); //
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 镜面反射光
    float specularStrength = 0.5; //
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); //
    vec3 specular = specularStrength * spec * lightColor;

    // 从纹理中采样颜色
    vec3 texColor = texture(texture1, TexCoords).rgb;

    // 结合光照和纹理颜色
    // vec3 result = (ambient + diffuse + specular) * objectColor;
    vec3 result = (ambient + diffuse + specular) * texColor; // 使用纹理颜色代替 objectColor
    FragColor = vec4(result, 1.0);
}