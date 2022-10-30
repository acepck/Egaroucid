/*
    Egaroucid for Web Project

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

/*
constexpr uint_fast8_t n_flip_pre_calc[N_8BIT][HW] = {
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
        n_flip_pre_calc[join_d7_line(player, u + t)][min(t, 7 - u)] + 
        n_flip_pre_calc[join_d9_line(player, u + 7 - t)][min(t, u)];
}
*/
inline int count_last_flip(uint64_t player, uint64_t opponent, const uint_fast8_t place){
    int t, u, p, o;
    int res = 0;

    t = place / HW;
    u = place % HW;
    p = (player >> (HW * t)) & 0b11111111;
    o = (opponent >> (HW * t)) & 0b11111111;
    res += n_flip_pre_calc[p][o][u];

    p = join_v_line(player, u);
    o = join_v_line(opponent, u);
    res += n_flip_pre_calc[p][o][t];

    t = place / HW;
    u = place % HW + t;
    if (u >= 2 && u <= 12){
        p = join_d7_line(player, u) & d7_mask[place];
        o = join_d7_line(opponent, u) & d7_mask[place];
        res += pop_count_uchar(flip_pre_calc[p][o][HW_M1 - t] & d7_mask[place]);
    }

    u -= t * 2;
    if (u >= -5 && u <= 5){
        p = join_d9_line(player, u) & d9_mask[place];
        o = join_d9_line(opponent, u) & d9_mask[place];
        res += pop_count_uchar(flip_pre_calc[p][o][t] & d9_mask[place]);
    }
    return res;
}