#include <unordered_map>
#include <filesystem>
#include <bvh/triangle.hpp>
#include <bvh/single_ray_traverser.hpp>
#include <bvh/primitive_intersectors.hpp>
#include "../utils/parse.hpp"
#include "../utils/build_bvh.hpp"
#include "../utils/cluster.hpp"

using Triangle = bvh::Triangle<float>;
using Ray = bvh::Ray<float>;
using Bvh = bvh::Bvh<float>;

int main() {
    std::vector<Triangle> triangles = parse_ply("sponza.ply");
    Bvh bvh = build_bvh(triangles);
    std::unordered_map<size_t, size_t> node_idx_to_cluster_idx = cluster(bvh, 1024);

    std::vector<std::tuple<size_t, size_t, bool>> edges;
    std::queue<size_t> queue;
    queue.push(0);
    while (!queue.empty()) {
        size_t curr = queue.front();
        queue.pop();
        if (!bvh.nodes[curr].is_leaf()) {
            size_t left_idx = bvh.nodes[curr].first_child_or_primitive;
            size_t right_idx = left_idx + 1;
            if (bvh.nodes[right_idx].bounding_box_proxy().half_area() > bvh.nodes[left_idx].bounding_box_proxy().half_area()) {
                std::swap(left_idx, right_idx);
            }

            edges.emplace_back(curr, left_idx, true);
            edges.emplace_back(curr, right_idx, false);
            queue.push(left_idx);
            queue.push(right_idx);
        }
    }

    bvh::ClosestPrimitiveIntersector<Bvh, Triangle> primitive_intersector(bvh, triangles.data());
    bvh::SingleRayTraverser<Bvh> traverser(bvh);

    std::ifstream ray_queries_file("ray_queries.bin", std::ios::in | std::ios::binary);
    std::filesystem::create_directory("graphs");
    float r[7];
    for (int i = 0; i < 5 && ray_queries_file.read(reinterpret_cast<char*>(&r), 7 * sizeof(float)); i++) {
        Ray ray(
                Vector3(r[0], r[1], r[2]),
                Vector3(r[3], r[4], r[5]),
                0.f,
                r[6]
        );
        std::unordered_map<size_t, bvh::SingleRayTraverser<Bvh>::NodeRecord> traversed;
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

        for (auto &t : traversed) {
            if (t.second == bvh::SingleRayTraverser<Bvh>::MISS) {
                dot_file << "\n    " << t.first << " [shape=star color=red label=\"\"]";
            } else if (t.second == bvh::SingleRayTraverser<Bvh>::PRUNED) {
                dot_file << "\n    " << t.first << " [shape=star color=blue label=\"\"]";
            }
        }

        for (auto &edge : edges) {
            dot_file << "\n    " << std::get<0>(edge) << " -> " << std::get<1>(edge);
            dot_file << " [";
            if (!std::get<2>(edge)) dot_file << "style=dashed ";

            assert(node_idx_to_cluster_idx.count(std::get<1>(edge)) > 0);
            size_t color_id = node_idx_to_cluster_idx[std::get<1>(edge)] % 5;
            if (color_id == 0) dot_file << "color=deeppink]";
            else if (color_id == 1) dot_file << "color=orange]";
            else if (color_id == 2) dot_file << "color=green]";
            else if (color_id == 3) dot_file << "color=blue]";
            else if (color_id == 4) dot_file << "color=blueviolet]";

            if (traversed.count(std::get<1>(edge)) != 0) {
                dot_file << "\n    " << std::get<0>(edge) << " -> " << std::get<1>(edge) << " [color=red penwidth=5]";
            }
        }

        dot_file << "\n}";
        dot_file.close();
    }
}
