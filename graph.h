#pragma once
#include "Resource.h"
#include <cmath>
#include <iostream>
#include <vector>

struct Vertex {
    int x_;
    int y_;

    std::vector<int> neighbors_;
};

struct Edge {
    Vertex first_;
    Vertex second_;

    float length_;
};

class Graph {
public:
    Graph(std::vector<std::vector<int>> input) {
        size_t size{ input[0].size() };

        for (size_t i{ 0 }; i < size; ++i) {
            vertex_array_.push_back({ input[0][i], input[1][i] });
        }

        std::vector<std::vector<float>> my_2d_vector(size,
            std::vector<float>(size, 0));

        distance_matrix_ = my_2d_vector;
    }
    void FormMatrix() {
        int i{ -1 };
        int j{ -1 };
        for (Vertex a : vertex_array_) {
            ++i;
            for (Vertex b : vertex_array_) {
                ++j;
                distance_matrix_[i][j] =
                    std::sqrt(pow(a.x_ - b.x_, 2) + pow(a.y_ - b.y_, 2));
            }
            j = -1;
        }
    }
    void PrintMatrix() {
        for (size_t i{ 0 }; i < distance_matrix_.size(); ++i) {
            for (size_t j{ 0 }; j < distance_matrix_.size(); ++j) {
                std::cout << distance_matrix_[i][j] << "\t";
            }
            std::cout << "\n";
        }
    }
    void BuildTreeEdgesPrim() {
        size_t matrix_size = distance_matrix_.size();
        if (matrix_size < 2) return;

        std::vector<bool> tree_has(matrix_size, false);
        tree_has[0] = true;
        size_t edges_added = 0;

        while (edges_added < matrix_size - 1) {
            float min_len = (std::numeric_limits<float>::max)();
            int best_i = -1;
            int best_j = -1;

            for (size_t i{ 0 }; i < matrix_size; ++i) {
                // Only consider i if it's in the tree AND hasn't hit its neighbor limit
                if (!tree_has[i] ||
                    vertex_array_[i].neighbors_.size() >= max_neighbors_) {
                    continue;
                }

                for (size_t j{ 0 }; j < matrix_size; ++j) {
                    // v must be outside the tree, have space for a neighbor, and have a
                    // valid distance
                    if (!tree_has[j] &&
                        vertex_array_[j].neighbors_.size() < max_neighbors_ &&
                        distance_matrix_[i][j] > 0) {
                        if (distance_matrix_[i][j] < min_len) {
                            min_len = distance_matrix_[i][j];
                            best_i = i;
                            best_j = j;
                        }
                    }
                }
            }

            if (best_j != -1) {
                tree_has[best_j] = true;
                tree_.push_back(
                    { vertex_array_[best_i], vertex_array_[best_j], min_len });
                vertex_array_[best_i].neighbors_.push_back(best_j);
                vertex_array_[best_j].neighbors_.push_back(best_i);
                ++edges_added;
            }
            else {
                // Either disconnected or no valid edges left that satisfy max_neighbors
                break;
            }
        }
    }
    void PrintVertices() {
        for (size_t i{ 0 }; i < vertex_array_.size(); ++i) {
            std::cout << "V " << i + 1 << "\tx: " << vertex_array_[i].x_
                << "\ty: " << vertex_array_[i].y_ << "\tneighbors: ";
            for (const auto& a : vertex_array_[i].neighbors_) {
                std::cout << a << " ";
            }
            std::cout << "\n";
        }
    }
    void PrintTree() {
        for (const auto& pair : tree_) {
            std::cout << "(" << pair.first_.x_ << "; " << pair.first_.y_ << ") -- "
                << "(" << pair.second_.x_ << "; " << pair.second_.y_ << ")\t"
                << pair.length_ << "\n";
        }
    }
    std::vector<Edge> GetTree() const { return tree_; }

private:
    std::vector<Vertex> vertex_array_;
    std::vector<std::vector<float>> distance_matrix_;
    std::vector<Edge> tree_;
    int max_neighbors_{ 4 };
};