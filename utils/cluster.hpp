#ifndef BVH_ANALYSIS_CLUSTER_HPP
#define BVH_ANALYSIS_CLUSTER_HPP

std::unordered_map<size_t, size_t> cluster(const Bvh &bvh, int num_clusters,
                                           double top_cluster_node_ratio = 0.001f) {
    std::unordered_map<size_t, size_t> node_idx_to_cluster_idx;

    size_t cluster_cnt = 0;
    size_t nodes_remaining;
    std::queue<size_t> queue;
    queue.push(0);
    while (!queue.empty()) {
        size_t curr = queue.front();
        queue.pop();
        node_idx_to_cluster_idx[curr] = 0;
        cluster_cnt++;
        if (!bvh.nodes[curr].is_leaf()) {
            size_t left_idx = bvh.nodes[curr].first_child_or_primitive;
            size_t right_idx = left_idx + 1;
            queue.push(left_idx);
            queue.push(right_idx);
        }
        if (cluster_cnt == size_t(double(bvh.node_count) * top_cluster_node_ratio)) {
            nodes_remaining = bvh.node_count - cluster_cnt;
            cluster_cnt = 0;
            break;
        }
    }

    size_t cluster_idx = 1;
    while (!queue.empty()) {
        size_t curr = queue.front();
        queue.pop();
        std::stack<size_t> stack;
        stack.push(curr);
        while (!stack.empty()) {
            curr = stack.top();
            stack.pop();
            node_idx_to_cluster_idx[curr] = cluster_idx;
            cluster_cnt += 1;
            if (!bvh.nodes[curr].is_leaf()) {
                size_t left_idx = bvh.nodes[curr].first_child_or_primitive;
                size_t right_idx = left_idx + 1;
                stack.push(right_idx);
                stack.push(left_idx);
            }
            if (cluster_cnt > nodes_remaining / (num_clusters - 1) && cluster_idx != num_clusters) {
                cluster_cnt = 0;
                cluster_idx++;
            }
        }
    }

    return node_idx_to_cluster_idx;
}

#endif //BVH_ANALYSIS_CLUSTER_HPP
