#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
    : m_VAO(),
      m_VBO(vertices.data(), vertices.size() * sizeof(Vertex)),
      m_IBO(indices.data(), indices.size())
{
    VertexBufferLayout layout;
    layout.Push<float>(3); 
    layout.Push<float>(4); 
    m_VAO.AddBuffer(m_VBO, layout);
}

void Mesh::Bind() const
{
    m_VAO.Bind();
    m_IBO.Bind();
}
unsigned int Mesh::GetIndexCount() const
{
    return m_IBO.GetCount();
}

void Mesh::updateVerts(const std::vector<Vertex> &vertices)
{
    m_VBO.Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
}