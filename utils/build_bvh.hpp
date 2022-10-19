#ifndef BVH_ANALYSIS_BUILD_BVH_HPP
#define BVH_ANALYSIS_BUILD_BVH_HPP

#include <bvh/triangle.hpp>
#include <bvh/sweep_sah_builder.hpp>

using Triangle = bvh::Triangle<float>;
using Bvh = bvh::Bvh<float>;

Bvh build_bvh(const std::vector<Triangle> &triangles) {
    Bvh bvh;
    auto [bboxes, centers] = bvh::compute_bounding_boxes_and_centers(triangles.data(), triangles.size());
    auto global_bbox = bvh::compute_bounding_boxes_union(bboxes.get(), triangles.size());
    std::cout << "global bounding box: ("
              << global_bbox.min[0] << ", " << global_bbox.min[1] << ", " << global_bbox.min[2] << "), ("
              << global_bbox.max[0] << ", " << global_bbox.max[1] << ", " << global_bbox.max[2] << ")" << std::endl;

    bvh::SweepSahBuilder<Bvh> builder(bvh);
    builder.build(global_bbox, bboxes.get(), centers.get(), triangles.size());

    return bvh;
}

#endif //BVH_ANALYSIS_BUILD_BVH_HPP
