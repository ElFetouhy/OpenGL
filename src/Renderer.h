#pragma once

#include <glew.h>
#include "Shader.h"
#include "Mesh.h"

class Renderer
{
public:
    void DrawMesh(const Mesh& mesh);
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
    void Clear() const;
};