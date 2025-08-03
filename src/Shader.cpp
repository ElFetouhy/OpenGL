#include "Shader.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "GLErrorHandler.h"

Shader::Shader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    // std::cout << "Vertex Shader Source:\n" << source.VertexSource << std::endl;
    // std::cout << "Fragment Shader Source:\n" << source.FragmentSource << std::endl;
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}


ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }
    std::cout << ss << std::endl;
    return {ss[0].str(), ss[1].str()};
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    GLint result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        GLint length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLint result;
    GLCall(glGetProgramiv(program, GL_LINK_STATUS, &result));
    if (result == GL_FALSE) {
        GLint length;
        GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetProgramInfoLog(program, length, &length, message));
        std::cout << "Failed to link program: " << message << std::endl;
        GLCall(glDeleteProgram(program));
        return 0;
    }
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocations(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocations(name), value));
}

void Shader::SetUniform2f(const std::string& name, float f0, float f1)
{
    GLCall(glUniform2f(GetUniformLocations(name), f0, f1));
}
void Shader::SetUniform3f(const std::string& name, float f0, float f1, float f2)
{
    GLCall(glUniform3f(GetUniformLocations(name), f0, f1, f2));
}
void Shader::SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3)
{
    GLCall(glUniform4f(GetUniformLocations(name), f0, f1, f2, f3));
}
void Shader::SetUniformMat3f(const std::string& name, const glm::mat3& matrix )
{
    GLCall(glUniformMatrix3fv(GetUniformLocations(name), 1, GL_FALSE, &matrix[0][0]));
}
void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix )
{
    GLCall(glUniformMatrix4fv(GetUniformLocations(name), 1, GL_FALSE, &matrix[0][0]));
}

GLint Shader::GetUniformLocations(const std::string& name) const
{
    //     because he did it wrong. And he did it wrong again:
    // if(auto &f = m.find("key"); f != m.end) return f->second;
    if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }

    GLCall( GLint location = glGetUniformLocation(m_RendererID, name.c_str()));
    if(location == -1)
    {
        std::cout << "Warning: uniform '"<< name << "' doesnt exist!" << std::endl;
    }
    m_UniformLocationCache[name] = location;
    return location;
}