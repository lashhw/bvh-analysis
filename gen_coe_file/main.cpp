#include <fstream>
#include <iomanip>
#include "../utils/parse.hpp"
#include "../utils/build_bvh.hpp"

unsigned int float_as_unsigned_int(float x) {
    return *reinterpret_cast<unsigned int*>(&x);
}

int main() {
    std::vector<Triangle> triangles = parse_ply("bun_zipper_res4.ply");
    Bvh bvh = build_bvh(triangles);

    std::ofstream nodes_coe_file("nodes.coe");
    nodes_coe_file << "memory_initialization_radix = 16;\nmemory_initialization_vector =\n";
    for (int i = 0; i < bvh.node_count; i++) {
        const Bvh::Node &node = bvh.nodes[i];
        if (i != 0) nodes_coe_file << ",\n";
        for (int j = 0; j < 6; j++)
            nodes_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(node.bounds[j]);
        nodes_coe_file << std::setfill('0') << std::setw(8) << std::hex << node.primitive_count;
        nodes_coe_file << std::setfill('0') << std::setw(8) << std::hex << node.first_child_or_primitive;
    }
    nodes_coe_file << ';';

    std::ofstream triangles_coe_file("triangles.coe");
    triangles_coe_file << "memory_initialization_radix = 16;\nmemory_initialization_vector =\n";
    for (int i = 0; i < triangles.size(); i++) {
        const Triangle &triangle = triangles[bvh.primitive_indices[i]];
        if (i != 0) triangles_coe_file << ",\n";
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.p0[0]);
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.p0[1]);
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.p0[2]);
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.e1[0]);
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.e1[1]);
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.e1[2]);
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.e2[0]);
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.e2[1]);
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.e2[2]);
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.n[0]);
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.n[1]);
        triangles_coe_file << std::setfill('0') << std::setw(8) << std::hex << float_as_unsigned_int(triangle.n[2]);
    }
    triangles_coe_file << ';';
}
