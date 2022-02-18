#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include "setting.hpp"
#include "common.hpp"
#include "board.hpp"
#include "evaluate.hpp"
#include "transpose_table.hpp"

using namespace std;

#define MID_MPC_MIN_DEPTH 2
#define MID_MPC_MAX_DEPTH 30
#define END_MPC_MIN_DEPTH 5
#define END_MPC_MAX_DEPTH 40
#define N_END_MPC_SCORE_DIV 11

#define MID_FAST_DEPTH 1
#define END_FAST_DEPTH 7
#define END_FAST_DEPTH1 6
#define END_FAST_DEPTH2 8
#define MID_TO_END_DEPTH 12

#define SCORE_UNDEFINED -INF

constexpr int cell_weight[HW2] = {
    18,  4,  16, 12, 12, 16,  4, 18,
     4,  2,   6,  8,  8,  6,  2,  4,
    16,  6,  14, 10, 10, 14,  6, 16,
    12,  8,  10,  0,  0, 10,  8, 12,
    12,  8,  10,  0,  0, 10,  8, 12,
    16,  6,  14, 10, 10, 14,  6, 16,
     4,  2,   6,  8,  8,  6,  2,  4,
    18,  4,  16, 12, 12, 16,  4, 18
};

constexpr int mpcd[41] = {
    0, 1, 0, 1, 2, 3, 2, 3, 4, 3, 
    4, 3, 4, 5, 4, 5, 6, 5, 6, 7, 
    6, 7, 6, 7, 8, 7, 8, 9, 8, 9, 
    10, 9, 10, 11, 12, 11, 12, 13, 14, 13, 
    14
};

constexpr double mpcsd[N_PHASES][MID_MPC_MAX_DEPTH - MID_MPC_MIN_DEPTH + 1]={
    {0.722, 0.776, 0.624, 0.852, 0.659, 0.751, 0.859, 0.444, 0.588, 0.442, 0.511, 0.587, 0.511, 0.464, 0.442, 0.444, 0.511, 0.482, 0.442, 0.576, 0.523, 0.588, 0.761, 0.509, 0.923, 1.083, 1.781, 1.586, 1.553},
    {1.414, 2.386, 1.042, 1.146, 0.565, 0.78, 0.721, 0.851, 0.806, 1.122, 0.83, 0.945, 1.215, 1.233, 0.794, 1.24, 0.908, 0.794, 1.042, 0.9, 1.218, 1.327, 1.412, 1.367, 1.864, 1.442, 1.56, 1.349, 1.599},      
    {2.81, 2.239, 2.359, 1.099, 0.806, 1.191, 0.751, 1.436, 0.833, 1.381, 0.737, 0.945, 1.049, 1.139, 1.018, 1.572, 1.116, 1.285, 1.283, 1.484, 1.504, 1.391, 1.398, 1.173, 1.165, 2.111, 1.442, 1.482, 2.315}, 
    {2.953, 1.248, 1.749, 1.278, 1.424, 1.316, 1.329, 1.098, 1.327, 1.341, 0.795, 0.899, 1.122, 1.445, 1.024, 1.043, 1.135, 1.316, 1.504, 1.275, 1.579, 1.063, 1.139, 1.359, 1.234, 1.435, 1.588, 1.153, 1.557},
    {3.569, 1.756, 1.373, 1.488, 1.56, 1.412, 1.071, 1.465, 1.122, 1.435, 1.518, 1.439, 1.25, 1.64, 1.465, 1.268, 1.503, 1.16, 1.373, 1.16, 1.472, 1.56, 1.341, 1.599, 1.579, 1.841, 1.622, 1.294, 1.367},      
    {3.895, 1.849, 1.276, 1.558, 1.285, 1.702, 1.348, 1.606, 1.398, 1.816, 1.599, 1.262, 1.412, 1.539, 1.496, 1.308, 1.233, 1.239, 1.226, 1.285, 1.382, 1.351, 1.504, 1.317, 1.209, 1.239, 1.993, 2.521, 2.018},
    {3.217, 2.226, 1.461, 1.628, 1.319, 1.511, 1.348, 1.285, 1.424, 1.285, 0.821, 1.103, 1.527, 1.732, 1.376, 1.469, 1.173, 1.222, 1.244, 1.316, 1.503, 1.53, 1.341, 1.348, 1.976, 1.926, 2.514, 1.72, 2.345},
    {2.919, 1.877, 1.207, 1.654, 1.933, 1.389, 1.476, 1.628, 1.398, 1.389, 1.526, 1.248, 1.073, 1.689, 1.279, 1.398, 0.955, 1.399, 1.341, 1.429, 1.494, 1.435, 1.627, 1.624, 1.53, 2.126, 2.197, 1.609, 1.775},
    {2.7, 2.305, 1.532, 1.53, 1.956, 2.067, 1.633, 1.622, 1.504, 1.916, 1.442, 1.285, 1.474, 1.473, 1.501, 1.444, 1.579, 1.432, 1.412, 1.666, 1.595, 1.706, 1.663, 1.967, 1.382, 0.983, 0.0, 0.0, 0.0},
    {2.658, 2.668, 1.408, 2.146, 1.574, 1.496, 1.583, 1.719, 1.373, 1.618, 1.245, 1.318, 1.341, 1.294, 1.381, 1.327, 1.268, 1.41, 1.341, 1.65, 2.575, 2.563, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {2.748, 4.344, 1.954, 1.345, 1.702, 1.395, 1.523, 1.956, 1.408, 1.75, 1.435, 1.501, 1.55, 1.758, 1.349, 2.086, 0.996, 0.548, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {3.618, 2.622, 2.75, 1.654, 1.778, 1.806, 1.527, 1.984, 1.589, 2.021, 1.998, 1.543, 1.663, 1.506, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {2.093, 2.808, 2.408, 2.16, 1.761, 1.789, 1.711, 2.415, 3.268, 3.521, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {3.796, 4.198, 3.46, 3.451, 2.685, 2.098, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {3.615, 2.16, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
};

constexpr int mpcd_final[41] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 
    3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 
    6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 
    8
};

constexpr double mpcsd_final[END_MPC_MAX_DEPTH - END_MPC_MIN_DEPTH + 1] = {
    4.597, 4.92, 5.343, 5.519, 5.971, 5.753, 5.66, 5.875, 5.794, 5.319, 5.123, 5.63, 5.236, 4.714, 5.283, 4.422, 4.904, 4.61, 4.836, 4.689, 4.865, 4.693, 4.182, 4.552, 4.523, 4.449, 5.256, 5.956, 6.934, 7.573, 7.676, 8.272, 9.088, 9.611, 10.535, 11.201
};

/*
constexpr double mpcsd_final[(END_MPC_MAX_DEPTH - END_MPC_MIN_DEPTH + 1) / 5 + 1][N_END_MPC_SCORE_DIV] = {
    {4.889, 5.386, 5.628, 5.239, 5.555, 5.182, 4.587, 5.971, 4.668, 3.582, 2.921},
    {5.497, 5.887, 6.061, 5.665, 6.19, 5.274, 5.581, 5.054, 6.273, 3.033, 3.262},
    {5.06, 5.722, 5.786, 5.6, 5.686, 5.062, 6.467, 6.544, 4.981, 4.231, 3.585},
    {4.642, 5.222, 5.253, 5.397, 5.88, 7.547, 5.39, 7.423, 6.19, 5.396, 4.168},
    {4.549, 5.144, 5.44, 5.543, 6.115, 6.138, 6.697, 6.611, 6.326, 5.581, 4.622},
    {6.297, 7.266, 7.616, 8.478, 10.048, 9.836, 11.373, 8.898, 7.225, 5.074, 4.509},
    {8.677, 10.56, 12.563, 13.18, 15.158, 14.175, 11.866, 10.332, 8.337, 7.182, 65},
    {10.142, 11.824, 15.938, 15.98, 16.75, 14.307, 5.244, 14.773, 65, 65, 65}
};
*/

unsigned long long can_be_flipped[HW2];

struct Search_result{
    int policy;
    int value;
    int depth;
    int nps;
};

class Search{
    public:
        Board board;
        bool skipped;
        bool use_mpc;
        double mpct;
        vector<int> vacant_list;
        unsigned long long n_nodes;
        int tt_parent_idx;
        int tt_child_idx;

    public:
        inline void pass(){
            board.p = 1 - board.p;
            skipped = true;
        }

        inline void undo_pass(){
            board.p = 1 - board.p;
            skipped = false;
        }
};

int cmp_vacant(int p, int q){
    return cell_weight[p] > cell_weight[q];
}

inline int stability_cut(Search *search, int *alpha, int *beta){
    int stab[2];
    calc_stability(&search->board, &stab[0], &stab[1]);
    int n_alpha = 2 * stab[search->board.p] - HW2;
    int n_beta = HW2 - 2 * stab[1 - search->board.p];
    if (*beta <= n_alpha)
        return n_alpha;
    if (n_beta <= *alpha)
        return n_beta;
    if (n_beta <= n_alpha)
        return n_alpha;
    *alpha = max(*alpha, n_alpha);
    *beta = min(*beta, n_beta);
    return SCORE_UNDEFINED;
}

inline void update_best_move(int best_moves[], int policy){
    best_moves[2] = best_moves[1];
    best_moves[1] = best_moves[0];
    best_moves[0] = policy;
}