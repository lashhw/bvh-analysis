#define TINYOBJLOADER_IMPLEMENTATION
#include "../third_party/tiny_obj_loader.h"
#include "../third_party/happly.h"

int main() {
    std::string filepath;
    std::cout << "filepath: ";
    std::cin >> filepath;

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

    std::vector<std::array<double, 3>> v_pos;
    std::vector<std::vector<size_t>> f_idx;
    for (const auto &shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            assert(shape.mesh.num_face_vertices[f] == 3);
            size_t idx_0 = shape.mesh.indices[index_offset + 0].vertex_index;
            size_t idx_1 = shape.mesh.indices[index_offset + 1].vertex_index;
            size_t idx_2 = shape.mesh.indices[index_offset + 2].vertex_index;
            f_idx.push_back({idx_0, idx_1, idx_2});
            index_offset += 3;
        }
    }

    for (int i = 0; i < attrib.vertices.size(); i += 3) {
        v_pos.push_back({attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]});
    }

    happly::PLYData plyOut;

    plyOut.addVertexPositions(v_pos);
    plyOut.addFaceIndices(f_idx);

    plyOut.write("output.ply", happly::DataFormat::ASCII);
}
