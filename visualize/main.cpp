#include <unordered_set>
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

constexpr int width = 10;
constexpr int height = 10;

int main() {
    std::vector<Triangle> triangles = parse_obj("sponza.obj");
    Bvh bvh = build_bvh(triangles);
    std::unordered_map<size_t, size_t> node_idx_to_cluster_idx = cluster(bvh, 4);

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

    std::ofstream image_file("image.ppm");
    image_file << "P3\n" << width << ' ' << height << "\n255\n";
    std::filesystem::create_directory("graphs");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            constexpr float origin_x = 5.f;
            constexpr float origin_y = 7.f;
            constexpr float origin_z = 0.f;
            float dir_x = -1.f;
            float dir_y = (8.f - 2.f * i / height) - origin_y;
            float dir_z = (1.f - 2.f * j / width) - origin_z;
            Ray ray(
                    Vector3(origin_x, origin_y, origin_z),
                    Vector3(dir_x, dir_y, dir_z),
                    0.f
            );

            std::unordered_set<size_t> traversed;
            std::vector<size_t> load_history;
            if (auto hit = traverser.traverse(ray, primitive_intersector, traversed, load_history)) {
                auto triangle_index = hit->primitive_index;
                float r = triangles[triangle_index].n[0];
                float g = triangles[triangle_index].n[1];
                float b = triangles[triangle_index].n[2];
                float length = sqrtf(r * r + g * g + b * b);
                r = (r / length + 1.f) / 2.f;
                g = (g / length + 1.f) / 2.f;
                b = (b / length + 1.f) / 2.f;
                image_file << std::clamp(int(256.f * r), 0, 255) << ' '
                           << std::clamp(int(256.f * g), 0, 255) << ' '
                           << std::clamp(int(256.f * b), 0, 255) << '\n';
            } else {
                image_file << "0 0 0\n";
            }

            std::string filepath = "graphs/bvh_" + std::to_string(i) + "_" + std::to_string(j) + ".dot";
            std::ofstream gv_file(filepath);
            gv_file << "digraph bvh {\n";
            gv_file << "    layout=twopi\n";
            gv_file << "    ranksep=2\n";
            gv_file << "    node [shape=point]\n";
            gv_file << "    edge [arrowhead=none penwidth=0.5]\n";
            gv_file << "    0 [shape=circle label=root]";

            for (auto &edge : edges) {
                gv_file << "\n    " << edge.first << " -> " << edge.second;
                if (traversed.count(edge.first) != 0) gv_file << " [color=red penwidth=5]";
                else if (node_idx_to_cluster_idx[edge.second] == 1) gv_file << " [color=yellow]";
                else if (node_idx_to_cluster_idx[edge.second] == 2) gv_file << " [color=green]";
                else if (node_idx_to_cluster_idx[edge.second] == 3) gv_file << " [color=blue]";
            }

            gv_file << "\n}";
            gv_file.close();
        }
    }
}
