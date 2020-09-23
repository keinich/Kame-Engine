#pragma once

#include <vector>
#include <unordered_map>
#include <Vertex.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

class Mesh {
public:
  Mesh() {}

  void create(const char* path) {
    tinyobj::attrib_t vertexAttributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnString;
    std::string errorString;

    bool success = tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, &warnString, &errorString, path);

    if (!success) {
      throw std::runtime_error(errorString);
    }

    std::unordered_map<Vertex, uint32_t> vertices;

    for (tinyobj::shape_t shape : shapes) {
      for (tinyobj::index_t index : shape.mesh.indices) {
        glm::vec3 pos = {
          vertexAttributes.vertices[3 * index.vertex_index + 0],
          vertexAttributes.vertices[3 * index.vertex_index + 2],
          vertexAttributes.vertices[3 * index.vertex_index + 1]
        };

        Vertex vert(pos, glm::vec3{ 1.0f, 0.0f, 0.2f }, glm::vec2{ 0.0f, 0.0f });

        if (vertices.count(vert) == 0) {
          vertices[vert] = vertices.size();
          _Vertices.push_back(vert);
        }
        _Indices.push_back(vertices[vert]);
      }
    }
  }

  std::vector<Vertex> getVertices() {
    return _Vertices;
  }

  std::vector<uint32_t> getIndices() {
    return _Indices;
  }

private:
  std::vector<Vertex> _Vertices;
  std::vector<uint32_t> _Indices;

};