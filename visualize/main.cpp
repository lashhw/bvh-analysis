#include <unordered_set>
#include <filesystem>
#include <bvh/triangle.hpp>
#include <bvh/single_ray_traverser.hpp>
#include <bvh/primitive_intersectors.hpp>
#include "../utils/parse.hpp"
#include "../utils/build_bvh.hpp"

using Triangle = bvh::Triangle<float>;
using Ray = bvh::Ray<float>;
using Bvh = bvh::Bvh<float>;

int main() {
    std::vector<Triangle> triangles = parse_ply("sponza.ply");
    Bvh bvh = build_bvh(triangles);

    std::vector<std::pair<size_t, size_t>> edges;
    std::queue<size_t> queue;
    queue.push(0);
    while (!queue.empty()) {
        size_t curr = queue.front();
        queue.pop();
        if (!bvh.nodes[curr].is_leaf()) {
            size_t left_idx = bvh.nodes[curr].first_child_or_primitive;
            size_t right_idx = left_idx + 1;
            edges.emplace_back(curr, left_idx);
            edges.emplace_back(curr, right_idx);
            queue.push(left_idx);
            queue.push(right_idx);
        }
    }

    bvh::ClosestPrimitiveIntersector<Bvh, Triangle> primitive_intersector(bvh, triangles.data());
    bvh::SingleRayTraverser<Bvh> traverser(bvh);

    std::ifstream ray_queries_file("ray_queries.bin", std::ios::in | std::ios::binary);
    std::filesystem::create_directory("graphs");
    float r[7];
    for (int i = 0; ray_queries_file.read(reinterpret_cast<char*>(&r), 7 * sizeof(float)); i++) {
        Ray ray(
                Vector3(r[0], r[1], r[2]),
                Vector3(r[3], r[4], r[5]),
                0.f,
                r[6]
        );
        std::unordered_set<size_t> traversed;
        std::vector<size_t> node_load_trace;
        traverser.traverse(ray, primitive_intersector, traversed, node_load_trace);

        std::string filepath = "graphs/bvh_" + std::to_string(i) + ".dot";
        std::ofstream dot_file(filepath);
        dot_file << "digraph bvh {\n";
        dot_file << "    layout=twopi\n";
        dot_file << "    ranksep=2\n";
        dot_file << "    root=0\n";
        dot_file << "    node [shape=point]\n";
        dot_file << "    edge [arrowhead=none penwidth=0.5]\n";
        dot_file << "    0 [shape=circle label=root]";

        for (auto &edge : edges) {
            dot_file << "\n    " << edge.first << " -> " << edge.second;
            if (traversed.count(edge.first) != 0) dot_file << " [color=red penwidth=5]";
        }

        dot_file << "\n}";
        dot_file.close();
    }
}
