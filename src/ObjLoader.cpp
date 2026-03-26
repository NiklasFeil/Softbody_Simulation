#include "ObjLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <filesystem>

Obj* ObjLoader::load(const char* path) {

    std::cout << "Loading object..."  << std::endl;

    std::vector<Eigen::Vector3d> positions;
    std::vector<std::tuple<unsigned, unsigned, double>> edges_with_length;
    std::vector<std::tuple<unsigned, unsigned, unsigned>> triangles;

    std::ifstream file;
    file.exceptions(std::ifstream::badbit);
    
    try {
        file.open(path);
        std::string line;

        while(std::getline(file, line)) {
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "v") {
                double x, y, z;
                ss >> x >> y >> z;
                positions.emplace_back(x, y, z);                
            }
            else if (prefix == "f") {
                unsigned i, j, k;
                ss >> i >> j >> k;

                // obj starts with index 1
                triangles.push_back({i - 1, j - 1, k - 1});
            }
        }

        // TODO: Make sure triangles all have the same winding order by constructing normals
        for (auto& triangle: triangles) {
            unsigned i = std::get<0>(triangle);
            unsigned j = std::get<1>(triangle);
            unsigned k = std::get<2>(triangle);

            Eigen::Vector3d v0 = positions[i];
            Eigen::Vector3d v1 = positions[j];
            Eigen::Vector3d v2 = positions[k];

            Eigen::Vector3d normal = (v1 - v0).cross(v2 - v0);

            if (normal.dot(v0) < 0) {
                std::swap(std::get<1>(triangle), std::get<2>(triangle));
            }
        }

        // Construct edges from triangles
        
        auto add_edge = [](unsigned a, unsigned b, std::set<std::pair<unsigned, unsigned>>& edge_set) {
            if (a > b) std::swap(a, b);
            edge_set.insert({a, b});
        };
        
        std::set<std::pair<unsigned, unsigned>> edges;
        for(const auto& triangle: triangles) {
            int i = std::get<0>(triangle);
            int j = std::get<1>(triangle);
            int k = std::get<2>(triangle);

            add_edge(i, j, edges);
            add_edge(j, k, edges);
            add_edge(i, k, edges);
        }

        // Construct edges with length
        for(const auto& edge: edges) {
            unsigned i = edge.first;
            unsigned j = edge.second;
            double length = (positions[i] - positions[j]).norm();
            edges_with_length.emplace_back(i, j, length);
        }
    }
    catch(std::ifstream::failure& e) {
        std::cout << "ERROR::OBJLOADER::FILE_NOT_SUCCESSFULLY_READ | Error code: " << e.code() << std::endl;
    }

    Obj* obj = new Obj;
    obj->positions = Eigen::VectorXd::Zero(3 * positions.size());
    for(int i = 0; i < positions.size(); i++) {
        obj->positions[3 * i    ] = positions[i].x();
        obj->positions[3 * i + 1] = positions[i].y();
        obj->positions[3 * i + 2] = positions[i].z();
    }

    obj->edges = edges_with_length;
    obj->triangles = triangles;

    return obj;

}