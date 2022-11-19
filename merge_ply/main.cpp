#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <array>
#include "../third_party/happly.h"
namespace fs = std::filesystem;

int main() {
    std::string folder_path;
    std::cout << "folder path: ";
    std::cin >> folder_path;

    std::vector<std::array<double, 3>> v_pos;
    std::vector<std::vector<size_t>> f_idx;

    for (const auto &entry : fs::directory_iterator(folder_path)) {
        happly::PLYData ply_data(entry.path());

        size_t offset = v_pos.size();
        std::vector<std::array<double, 3>> v_pos_tmp = ply_data.getVertexPositions();
        v_pos.insert(v_pos.end(), v_pos_tmp.begin(), v_pos_tmp.end());

        std::vector<std::vector<size_t>> f_idx_tmp = ply_data.getFaceIndices<size_t>();
        for (auto &x : f_idx_tmp)
            for (auto &y : x)
                y += offset;
        f_idx.insert(f_idx.end(), f_idx_tmp.begin(), f_idx_tmp.end());
    }

    happly::PLYData ply_out;
    ply_out.addVertexPositions(v_pos);
    ply_out.addFaceIndices(f_idx);
    ply_out.write("output.ply");
}
