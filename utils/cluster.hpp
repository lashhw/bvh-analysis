#ifndef BVH_ANALYSIS_CLUSTER_HPP
#define BVH_ANALYSIS_CLUSTER_HPP

void preorder_cluster(const Bvh &bvh, size_t nodes_per_cluster,
                      size_t curr_node_idx,
                      size_t &counter,
                      std::unordered_map<size_t, size_t> &node_idx_to_cluster_idx) {
    if (!bvh.nodes[curr_node_idx].is_leaf()) {
        size_t left_node_idx = bvh.nodes[curr_node_idx].first_child_or_primitive;
        size_t right_node_idx = left_node_idx + 1;

        if (bvh.nodes[right_node_idx].bounding_box_proxy().half_area() >
            bvh.nodes[left_node_idx].bounding_box_proxy().half_area()) {
            std::swap(left_node_idx, right_node_idx);
        }

        node_idx_to_cluster_idx[left_node_idx] = counter / nodes_per_cluster;
        node_idx_to_cluster_idx[right_node_idx] = counter / nodes_per_cluster;
        counter++;

        preorder_cluster(bvh, nodes_per_cluster, left_node_idx, counter, node_idx_to_cluster_idx);
        preorder_cluster(bvh, nodes_per_cluster, right_node_idx, counter, node_idx_to_cluster_idx);
    }
}

std::unordered_map<size_t, size_t> cluster(const Bvh &bvh, size_t nodes_per_cluster) {
    std::unordered_map<size_t, size_t> node_idx_to_cluster_idx;

    size_t counter = 0;
    preorder_cluster(bvh, nodes_per_cluster, 0, counter, node_idx_to_cluster_idx);

    return node_idx_to_cluster_idx;
}

#endif //BVH_ANALYSIS_CLUSTER_HPP
