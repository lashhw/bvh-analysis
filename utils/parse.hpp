#ifndef BVH_ANALYSIS_PARSE_HPP
#define BVH_ANALYSIS_PARSE_HPP

#include <bvh/triangle.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include "../third_party/tiny_obj_loader.h"
#include "../third_party/happly.h"

using Vector3  = bvh::Vector3<float>;
using Triangle = bvh::Triangle<float>;

std::vector<Triangle> parse_obj(const std::string &filepath) {
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        return {};
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    std::vector<Triangle> triangles;

    // Loop over shapes
    for (const auto &shape : shapes) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            assert(shape.mesh.num_face_vertices[f] == 3);

            // Loop over vertices in the face.
            tinyobj::index_t idx_1 = shape.mesh.indices[index_offset];
            float vx_1 = attrib.vertices[3 * size_t(idx_1.vertex_index) + 0];
            float vy_1 = attrib.vertices[3 * size_t(idx_1.vertex_index) + 1];
            float vz_1 = attrib.vertices[3 * size_t(idx_1.vertex_index) + 2];
            index_offset += 1;

            tinyobj::index_t idx_2 = shape.mesh.indices[index_offset];
            float vx_2 = attrib.vertices[3 * size_t(idx_2.vertex_index) + 0];
            float vy_2 = attrib.vertices[3 * size_t(idx_2.vertex_index) + 1];
            float vz_2 = attrib.vertices[3 * size_t(idx_2.vertex_index) + 2];
            index_offset += 1;

            tinyobj::index_t idx_3 = shape.mesh.indices[index_offset];
            float vx_3 = attrib.vertices[3 * size_t(idx_3.vertex_index) + 0];
            float vy_3 = attrib.vertices[3 * size_t(idx_3.vertex_index) + 1];
            float vz_3 = attrib.vertices[3 * size_t(idx_3.vertex_index) + 2];
            index_offset += 1;

            triangles.emplace_back(Vector3(vx_1, vy_1, vz_1),
                                   Vector3(vx_2, vy_2, vz_2),
                                   Vector3(vx_3, vy_3, vz_3));
        }
    }

    return triangles;
}

std::vector<Triangle> parse_ply(const std::string &filepath) {
    happly::PLYData ply_data(filepath);
    std::vector<std::array<double, 3>> v_pos = ply_data.getVertexPositions();
    std::vector<std::vector<size_t>> f_idx = ply_data.getFaceIndices<size_t>();

    std::vector<Triangle> triangles;
    for (auto &face : f_idx) {
        triangles.emplace_back(Vector3(v_pos[face[0]][0], v_pos[face[0]][1], v_pos[face[0]][2]),
                               Vector3(v_pos[face[1]][0], v_pos[face[1]][1], v_pos[face[1]][2]),
                               Vector3(v_pos[face[2]][0], v_pos[face[2]][1], v_pos[face[2]][2]));
    }
    return triangles;
}

#endif //BVH_ANALYSIS_PARSE_HPP
