#include <unordered_map>
#include <unordered_set>
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
    std::cout << "BVH has " << bvh.node_count << " nodes" << std::endl;

    std::unordered_map<size_t, size_t> node_idx_to_cluster_idx = cluster(bvh, 4);

    bvh::ClosestPrimitiveIntersector<Bvh, Triangle> primitive_intersector(bvh, triangles.data());
    bvh::SingleRayTraverser<Bvh> traverser(bvh);

    std::unordered_map<size_t, size_t> cluster_stat;
    std::ifstream ray_queries_file("ray_queries.bin", std::ios::in | std::ios::binary);
    std::ofstream node_load_trace_file("node_load_trace.txt");
    float r[7];
    while (ray_queries_file.read(reinterpret_cast<char*>(&r), 7 * sizeof(float))) {
        Ray ray(
                Vector3(r[0], r[1], r[2]),
                Vector3(r[3], r[4], r[5]),
                0.f,
                r[6]
        );
        std::unordered_set<size_t> traversed;
        std::vector<size_t> node_load_trace;
        traverser.traverse(ray, primitive_intersector, traversed, node_load_trace);

        for (auto &n : node_load_trace) {
            cluster_stat[node_idx_to_cluster_idx[n]]++;
            node_load_trace_file << n << " " << node_idx_to_cluster_idx[n] << '\n';
        }
    }

    for (auto &c : cluster_stat) {
        std::cout << c.first << ": " << c.second << std::endl;
    }
}
