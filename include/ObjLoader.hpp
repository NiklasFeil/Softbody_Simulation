#pragma once
#include <vector>
#include <Eigen/Dense>

struct Obj {
    Eigen::VectorXd positions;
    std::vector<std::tuple<unsigned, unsigned, double>> edges;
    std::vector<std::tuple<unsigned, unsigned, unsigned>> triangles;
};

struct ObjLoader {
    static Obj* load(const char* path);
};