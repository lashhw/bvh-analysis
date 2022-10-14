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

constexpr int width = 10;
constexpr int height = 10;

int main() {
    std::vector<Triangle> triangles = parse_obj("sponza.obj");
    Bvh bvh = build_bvh(triangles);

    bvh::ClosestPrimitiveIntersector<Bvh, Triangle> primitive_intersector(bvh, triangles.data());
    bvh::SingleRayTraverser<Bvh> traverser(bvh);

    std::vector<size_t> load_history;
    std::ofstream image_file("image.ppm");
    image_file << "P3\n" << width << ' ' << height << "\n255\n";
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
        }

        std::ofstream history_file("history.txt");
        for (auto &h : load_history) history_file << h << '\n';
    }
}
