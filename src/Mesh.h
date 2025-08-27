#pragma once
#include <vector>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "glm.hpp"

struct Vertex
{
    glm::vec3 Position;
    glm::vec4 Color;
};

class Mesh
{
public:
    Mesh( const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh() = default;
    
    void updateVerts(const std::vector<Vertex> &vertices);
    void Bind() const;
    unsigned int GetIndexCount() const; // inline ?
private:
    VertexArray m_VAO;
    VertexBuffer m_VBO;
    IndexBuffer m_IBO;
};