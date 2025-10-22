#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // 包含 glm::value_ptr

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // 程序ID
    unsigned int ID;

    // 构造函数读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. 从文件路径中获取顶点/片段着色器源代码
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // 保证ifstream对象可以抛出异常结构：
        // 设置了failbit和badbit异常位，failbit用于文件读取失败，badbit用于其他严重错误
        // 使用位或操作符组合这些标志，表示当任一情况发生时抛出异常
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // 检查文件是否存在
            std::ifstream vertexCheck(vertexPath);
            std::ifstream fragmentCheck(fragmentPath);
            if (!vertexCheck.good()) {
                std::cout << "ERROR::SHADER::VERTEX_FILE_NOT_FOUND: " << vertexPath << std::endl;
                return;
            }
            if (!fragmentCheck.good()) {
                std::cout << "ERROR::SHADER::FRAGMENT_FILE_NOT_FOUND: " << fragmentPath << std::endl;
                return;
            }
            vertexCheck.close();
            fragmentCheck.close();
            
            // 打开文件
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // 读取文件的缓冲内容到数据流中
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();		
            // 关闭文件处理器
            vShaderFile.close();
            fShaderFile.close();
            // 转换数据流到string
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            
            // 检查着色器代码是否为空
            if (vertexCode.empty()) {
                std::cout << "ERROR::SHADER::VERTEX_FILE_EMPTY: " << vertexPath << std::endl;
                return;
            }
            if (fragmentCode.empty()) {
                std::cout << "ERROR::SHADER::FRAGMENT_FILE_EMPTY: " << fragmentPath << std::endl;
                return;
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
            std::cout << "Vertex Path: " << vertexPath << std::endl;
            std::cout << "Fragment Path: " << fragmentPath << std::endl;
            return;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        
        // 2. 编译着色器
        unsigned int vertex, fragment;

        /*
        有五类型的着色器：
        1. 顶点着色器（GL_VERTEX_SHADER）
        2. 片段着色器（GL_FRAGMENT_SHADER）
        3. 几何着色器（GL_GEOMETRY_SHADER）
        4. 曲面细分控制着色器（GL_TESS_CONTROL_SHADER），曲面细分评估着色器（GL_TESS_EVALUATION_SHADER）
        5. 计算着色器（GL_COMPUTE_SHADER）
        着色器之间的关系：顶点着色器 --> 曲面细分控制着色器 --> 曲面细分评估着色器 --> 几何着色器 --> 片段着色器

        顶点着色器必须输出一个名为gl_Position的变量。
        片段着色器必须输出一个四维向量，表示片段的颜色。
        前一个着色器的输出必须与后一个着色器的输入在类型、名称和数量上匹配。
        即使跳过某些着色器，也必须保持原先定义的着色器顺序

        每个CUDA Core都有自己的寄存器，用于存储临时变量和中间结果。

        CUDA全称是Compute Unified Device Architecture。
        */
        
        // 顶点着色器
        vertex = glCreateShader(GL_VERTEX_SHADER);
        // 第一个参数：指定要编译的着色器对象的ID
        // 第二个参数：只有1个源代码字符串
        // 第三个参数：指向字符串的指针的地址
        // 第四个参数：指向整数的指针，指定每个字符串的长度（如果为NULL，则默认认为字符串以NULL结尾）
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        // 编译顶点着色器
        glCompileShader(vertex);
        // 检查顶点着色器编译是否成功
        checkCompileErrors(vertex, "VERTEX");
        
        // 片段着色器
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        
        // 着色器程序
        ID = glCreateProgram();
        // 一个程序对象可以链接多个着色器对象
        // 链接过程会检查各个着色器之间的兼容性，比如顶点着色器的输出是否与片段着色器的输入匹配
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        
        // 删除着色器，因为它们已经链接到我们的程序中了，不再需要
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // 激活着色器
    // 使用此着色器程序进行渲染
    // glUseProgram()函数将当前渲染状态设置为使用此着色器程序
    // 后续的渲染调用将使用此着色器程序中定义的顶点和片段处理逻辑
    void use() const
    { 
        glUseProgram(ID); 
    }

    // uniform工具函数
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value)); 
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }
    // 设置三维向量uniform变量的重载函数1：接受glm::vec3类型参数
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        // 获取着色器中uniform变量的位置，然后传递一个三维向量值
        // glGetUniformLocation: 获取着色器程序中指定名称的uniform变量的位置索引
        //   - ID: 着色器程序的ID
        //   - name.c_str(): uniform变量的名称，转换为C风格字符串
        // glUniform3fv: 设置三维向量uniform变量的值（数组版本）
        //   - 第一个参数: uniform变量的位置索引
        //   - 第二个参数: 要传递的向量数量（这里是1个向量）
        //   - 第三个参数: 指向向量数据的指针
        // glm::value_ptr: 获取glm::vec3对象的底层数组指针
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }
    
    // 设置三维向量uniform变量的重载函数2：接受三个单独的float参数
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        // 获取着色器中uniform变量的位置，然后传递三个单独的浮点数值
        // glGetUniformLocation: 获取着色器程序中指定名称的uniform变量的位置索引
        //   - ID: 着色器程序的ID
        //   - name.c_str(): uniform变量的名称，转换为C风格字符串
        // glUniform3f: 设置三维向量uniform变量的值（单独值版本）
        //   - 第一个参数: uniform变量的位置索引
        //   - 第二个参数: 向量的x分量
        //   - 第三个参数: 向量的y分量
        //   - 第四个参数: 向量的z分量
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value)); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    // 设置4x4矩阵uniform变量的函数
    // 用于将模型、视图或投影矩阵等变换矩阵传递给着色器
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        // 将4x4矩阵传递给着色器程序中的uniform变量
        // glGetUniformLocation: 获取着色器程序中指定名称的uniform变量的位置索引
        //   - ID: 着色器程序的ID
        //   - name.c_str(): uniform变量的名称，转换为C风格字符串
        // glUniformMatrix4fv: 设置4x4矩阵uniform变量的值
        //   - 第一个参数: uniform变量的位置索引
        //   - 第二个参数: 要传递的矩阵数量（这里是1个矩阵）
        //   - 第三个参数: 是否需要对矩阵进行转置（GL_FALSE表示不转置，因为GLM库的矩阵布局与OpenGL期望的布局一致）
        //   - 第四个参数: 指向矩阵数据的指针
        // glm::value_ptr: 获取glm::mat4对象的底层数组指针，直接访问矩阵的内部数据
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

private:
    // 用于检查着色器编译/链接错误的工具函数
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success, logLength;
        logLength = 128;
        GLchar infoLog[logLength];
        if (type!= "PROGRAM")
        {
            // iv后缀i=integer，v=vector
            // 获取着色器的特定参数值
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                // 1024：指定infoLog的最大长度，确保不会溢出
                // 返回信息长度的指针
                glGetShaderInfoLog(shader, logLength, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif