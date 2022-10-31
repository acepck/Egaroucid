/*
    Egaroucid Project

    @date 2021-2022
    @author Takuto Yamana (a.k.a Nyanyan)
    @license GPL-3.0 license
*/

#pragma once
#include <iostream>
#include "setting.hpp"
#include "common.hpp"
#include "bit.hpp"

using namespace std;

constexpr int_fast8_t n_flip_pre_calc[N_8BIT][HW] = {
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 2, 3, 4, 5, 6}, {0, 0, 0, 1, 2, 3, 4, 5}, {0, 0, 0, 1, 2, 3, 4, 5}, {1, 0, 0, 0, 1, 2, 3, 4}, {0, 0, 0, 0, 1, 2, 3, 4}, {0, 0, 0, 0, 1, 2, 3, 4}, {0, 0, 0, 0, 1, 2, 3, 4}, 
    {2, 1, 0, 0, 0, 1, 2, 3}, {0, 1, 1, 0, 0, 1, 2, 3}, {0, 0, 0, 0, 0, 1, 2, 3}, {0, 0, 0, 0, 0, 1, 2, 3}, {1, 0, 0, 0, 0, 1, 2, 3}, {0, 0, 0, 0, 0, 1, 2, 3}, {0, 0, 0, 0, 0, 1, 2, 3}, {0, 0, 0, 0, 0, 1, 2, 3},
    {3, 2, 1, 0, 0, 0, 1, 2}, {0, 2, 2, 2, 0, 0, 1, 2}, {0, 0, 1, 1, 0, 0, 1, 2}, {0, 0, 1, 1, 0, 0, 1, 2}, {1, 0, 0, 0, 0, 0, 1, 2}, {0, 0, 0, 0, 0, 0, 1, 2}, {0, 0, 0, 0, 0, 0, 1, 2}, {0, 0, 0, 0, 0, 0, 1, 2},
    {2, 1, 0, 0, 0, 0, 1, 2}, {0, 1, 1, 0, 0, 0, 1, 2}, {0, 0, 0, 0, 0, 0, 1, 2}, {0, 0, 0, 0, 0, 0, 1, 2}, {1, 0, 0, 0, 0, 0, 1, 2}, {0, 0, 0, 0, 0, 0, 1, 2}, {0, 0, 0, 0, 0, 0, 1, 2}, {0, 0, 0, 0, 0, 0, 1, 2},
    {4, 3, 2, 1, 0, 0, 0, 1}, {0, 3, 3, 3, 3, 0, 0, 1}, {0, 0, 2, 2, 2, 0, 0, 1}, {0, 0, 2, 2, 2, 0, 0, 1}, {1, 0, 0, 1, 1, 0, 0, 1}, {0, 0, 0, 1, 1, 0, 0, 1}, {0, 0, 0, 1, 1, 0, 0, 1}, {0, 0, 0, 1, 1, 0, 0, 1},
    {2, 1, 0, 0, 0, 0, 0, 1}, {0, 1, 1, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1},
    {3, 2, 1, 0, 0, 0, 0, 1}, {0, 2, 2, 2, 0, 0, 0, 1}, {0, 0, 1, 1, 0, 0, 0, 1}, {0, 0, 1, 1, 0, 0, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1}, 
    {2, 1, 0, 0, 0, 0, 0, 1}, {0, 1, 1, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 1},
    {5, 4, 3, 2, 1, 0, 0, 0}, {0, 4, 4, 4, 4, 4, 0, 0}, {0, 0, 3, 3, 3, 3, 0, 0}, {0, 0, 3, 3, 3, 3, 0, 0}, {1, 0, 0, 2, 2, 2, 0, 0}, {0, 0, 0, 2, 2, 2, 0, 0}, {0, 0, 0, 2, 2, 2, 0, 0}, {0, 0, 0, 2, 2, 2, 0, 0},
    {2, 1, 0, 0, 1, 1, 0, 0}, {0, 1, 1, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0},
    {3, 2, 1, 0, 0, 0, 0, 0}, {0, 2, 2, 2, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, 
    {2, 1, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {4, 3, 2, 1, 0, 0, 0, 0}, {0, 3, 3, 3, 3, 0, 0, 0}, {0, 0, 2, 2, 2, 0, 0, 0}, {0, 0, 2, 2, 2, 0, 0, 0}, {1, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0},
    {2, 1, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {3, 2, 1, 0, 0, 0, 0, 0}, {0, 2, 2, 2, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {2, 1, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, 
    {6, 5, 4, 3, 2, 1, 0, 0}, {0, 5, 5, 5, 5, 5, 5, 0}, {0, 0, 4, 4, 4, 4, 4, 0}, {0, 0, 4, 4, 4, 4, 4, 0}, {1, 0, 0, 3, 3, 3, 3, 0}, {0, 0, 0, 3, 3, 3, 3, 0}, {0, 0, 0, 3, 3, 3, 3, 0}, {0, 0, 0, 3, 3, 3, 3, 0},
    {2, 1, 0, 0, 2, 2, 2, 0}, {0, 1, 1, 0, 2, 2, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 0}, {1, 0, 0, 0, 2, 2, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 0},
    {3, 2, 1, 0, 0, 1, 1, 0}, {0, 2, 2, 2, 0, 1, 1, 0}, {0, 0, 1, 1, 0, 1, 1, 0}, {0, 0, 1, 1, 0, 1, 1, 0}, {1, 0, 0, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 0, 1, 1, 0},
    {2, 1, 0, 0, 0, 1, 1, 0}, {0, 1, 1, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 0, 1, 1, 0}, {1, 0, 0, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 0, 1, 1, 0},
    {4, 3, 2, 1, 0, 0, 0, 0}, {0, 3, 3, 3, 3, 0, 0, 0}, {0, 0, 2, 2, 2, 0, 0, 0}, {0, 0, 2, 2, 2, 0, 0, 0}, {1, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, 
    {2, 1, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {3, 2, 1, 0, 0, 0, 0, 0}, {0, 2, 2, 2, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {2, 1, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {5, 4, 3, 2, 1, 0, 0, 0}, {0, 4, 4, 4, 4, 4, 0, 0}, {0, 0, 3, 3, 3, 3, 0, 0}, {0, 0, 3, 3, 3, 3, 0, 0}, {1, 0, 0, 2, 2, 2, 0, 0}, {0, 0, 0, 2, 2, 2, 0, 0}, {0, 0, 0, 2, 2, 2, 0, 0}, {0, 0, 0, 2, 2, 2, 0, 0},
    {2, 1, 0, 0, 1, 1, 0, 0}, {0, 1, 1, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0}, 
    {3, 2, 1, 0, 0, 0, 0, 0}, {0, 2, 2, 2, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {2, 1, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {4, 3, 2, 1, 0, 0, 0, 0}, {0, 3, 3, 3, 3, 0, 0, 0}, {0, 0, 2, 2, 2, 0, 0, 0}, {0, 0, 2, 2, 2, 0, 0, 0}, {1, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0},
    {2, 1, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {3, 2, 1, 0, 0, 0, 0, 0}, {0, 2, 2, 2, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, 
    {2, 1, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}
};

inline int_fast8_t count_last_flip(uint64_t player, uint64_t opponent, const uint_fast8_t place){
    const int t = place >> 3;
    const int u = place & 7;
    return
        n_flip_pre_calc[join_h_line(player, t)][u] + 
        n_flip_pre_calc[join_v_line(player, u)][t] + 
        n_flip_pre_calc[join_d7_line2(player, u + t)][min(t, 7 - u)] + 
        n_flip_pre_calc[join_d9_line2(player, u + 7 - t)][min(t, u)];
}
