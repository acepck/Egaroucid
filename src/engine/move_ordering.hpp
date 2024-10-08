/*
    Egaroucid Project

    @file move_ordering.hpp
        Move ordering for each search algorithm
    @date 2021-2023
    @author Takuto Yamana
    @license GPL-3.0 license
*/

#pragma once
#include <iostream>
#include <vector>
#if USE_SIMD
    #ifdef _MSC_VER
        #include <intrin.h>
    #else
        #include <x86intrin.h>
    #endif
#endif
#include "setting.hpp"
#include "common.hpp"
#include "board.hpp"
#include "search.hpp"
#include "midsearch.hpp"
#if USE_LIGHT_EVALUATION
#include "midsearch_light.hpp"
#endif
#include "evaluate.hpp"
#include "stability.hpp"
#include "level.hpp"
#include "transposition_table.hpp"

/*
    @brief if wipeout found, it must be searched first.
*/
#define W_WIPEOUT 100000000
#define W_1ST_MOVE 10000000
#define W_2ND_MOVE 1000000
#define MO_OFFSET_L_PM 38

/*
    @brief constants for move ordering
*/
#if TUNE_MOVE_ORDERING
    #define N_MOVE_ORDERING_PARAM 15
    int move_ordering_param_array[N_MOVE_ORDERING_PARAM] = {
        35, 17, 485, 269, 94, 
        17, 8, 204, 7, 25, 
        40, 12, 
        18, 17, 300
    };

    #define W_MOBILITY                  move_ordering_param_array[0]
    #define W_POTENTIAL_MOBILITY        move_ordering_param_array[1]
    #define W_TT_BONUS                  move_ordering_param_array[2]
    #define W_VALUE                     move_ordering_param_array[3]
    #define W_VALUE_DEEP_ADDITIONAL     move_ordering_param_array[4]

    #define W_NWS_MOBILITY              move_ordering_param_array[5]
    #define W_NWS_POTENTIAL_MOBILITY    move_ordering_param_array[6]
    #define W_NWS_TT_BONUS              move_ordering_param_array[7]
    #define W_NWS_VALUE                 move_ordering_param_array[8]
    #define W_NWS_VALUE_DEEP_ADDITIONAL move_ordering_param_array[9]

    #define W_END_NWS_MOBILITY          move_ordering_param_array[10]
    #define W_END_NWS_VALUE             move_ordering_param_array[11]

    #define W_END_NWS_SIMPLE_MOBILITY   move_ordering_param_array[12]
    #define W_END_NWS_SIMPLE_PARITY     move_ordering_param_array[13]
    #define W_END_NWS_SIMPLE_TT_BONUS   move_ordering_param_array[14]

    #define MOVE_ORDERING_PARAM_START 0
    #define MOVE_ORDERING_PARAM_END 9
#else
    // midgame search
    #define W_MOBILITY 35
    #define W_POTENTIAL_MOBILITY 17
    #define W_TT_BONUS 485
    #define W_VALUE 269
    #define W_VALUE_DEEP_ADDITIONAL 94

    // midgame null window search
    #define W_NWS_MOBILITY 17
    #define W_NWS_POTENTIAL_MOBILITY 8
    #define W_NWS_TT_BONUS 204
    #define W_NWS_VALUE 7
    #define W_NWS_VALUE_DEEP_ADDITIONAL 25

    // endgame null window search
    #define W_END_NWS_MOBILITY 40
    #define W_END_NWS_VALUE 12

    // endgame simple null window search
    #define W_END_NWS_SIMPLE_MOBILITY 18
    #define W_END_NWS_SIMPLE_PARITY 17
    #define W_END_NWS_SIMPLE_TT_BONUS 300
#endif

#define MOVE_ORDERING_VALUE_OFFSET_ALPHA 18
#define MOVE_ORDERING_VALUE_OFFSET_BETA 12
#define MOVE_ORDERING_NWS_VALUE_OFFSET_ALPHA 16
#define MOVE_ORDERING_NWS_VALUE_OFFSET_BETA 6

#define MOVE_ORDERING_MPC_LEVEL MPC_88_LEVEL

int nega_alpha_eval1(Search *search, int alpha, int beta, bool skipped);
inline int nega_alpha_eval1_move_ordering_mid(Search *search, int alpha, int beta, bool skipped);
int nega_scout(Search *search, int alpha, int beta, int depth, bool skipped, uint64_t legal, bool is_end_search, const bool *searching);
#if USE_LIGHT_EVALUATION
inline int nega_alpha_light_eval1(Search *search, int alpha, int beta, bool skipped);
int nega_alpha_light(Search *search, int alpha, int beta, int depth, bool skipped, uint64_t legal, const bool *searching);
#endif
inline bool transposition_table_get_value(Search *search, uint32_t hash, int *l, int *u);

/*
    @brief Get number of corner mobility

    Optimized for corner mobility

    @param legal                legal moves as a bitboard
    @return number of legal moves on corners
*/
#if USE_BUILTIN_POPCOUNT
    inline int get_corner_n_moves(uint64_t legal) {
        return pop_count_ull(legal & 0x8100000000000081ULL);
    }
#else
    inline int get_corner_n_moves(uint64_t legal) {
        int res = (int)((legal & 0b10000001ULL) + ((legal >> 56) & 0b10000001ULL));
        return (res & 0b11) + (res >> 7);
    }
#endif

/*
    @brief Get a weighted mobility

    @param legal                legal moves as a bitboard
    @return weighted mobility
*/
inline int get_weighted_n_moves(uint64_t legal) {
    return pop_count_ull(legal) * 2 + get_corner_n_moves(legal);
}

#ifdef USE_SIMD
inline int get_n_moves_cornerX2(uint64_t legal) {
    return pop_count_ull(legal) + get_corner_n_moves(legal);
}
#else
inline int get_n_moves_cornerX2(uint64_t legal) {
    uint64_t b = legal;
    uint64_t c = b & 0x0100000000000001ull;
    b -= (b >> 1) & 0x1555555555555515ull;
    b = (b & 0x3333333333333333ull) + ((b >> 2) & 0x3333333333333333ull);
    b += c;
    b += (b >> 4);
    b &= 0x0f0f0f0f0f0f0f0full;
    b *= 0x0101010101010101ULL;
    return b >> 56;
}
#endif

/*
    @brief Get potential mobility

    Same idea as surround in evaluation function

    @param discs                a bitboard representing discs
    @param empties              a bitboard representing empty squares
    @return potential mobility
*/
#ifdef CALC_SURROUND_FUNCTION
    #define get_potential_mobility(discs, empties) calc_surround(discs, empties)
#elif USE_SIMD
    inline int get_potential_mobility(uint64_t discs, uint64_t empties) {
        __m256i eval_surround_mask = _mm256_set_epi64x(0x7E7E7E7E7E7E7E7EULL, 0x00FFFFFFFFFFFF00ULL, 0x007E7E7E7E7E7E00ULL, 0x007E7E7E7E7E7E00ULL);
        __m128i eval_surround_shift1879 = _mm_set_epi32(1, HW, HW_M1, HW_P1);
        __m256i pl = _mm256_set1_epi64x(discs);
        pl = _mm256_and_si256(pl, eval_surround_mask);
        pl = _mm256_or_si256(_mm256_sllv_epi64(pl, eval_surround_shift1879), _mm256_srlv_epi64(pl, eval_surround_shift1879));
        __m128i res = _mm_or_si128(_mm256_castsi256_si128(pl), _mm256_extracti128_si256(pl, 1));
        res = _mm_or_si128(res, _mm_shuffle_epi32(res, 0x4e));
        return pop_count_ull(_mm_cvtsi128_si64(res) & empties);
    }
#else
    inline int get_potential_mobility(uint64_t discs, uint64_t empties) {
        uint64_t hmask = discs & 0x7E7E7E7E7E7E7E7EULL;
        uint64_t vmask = discs & 0x00FFFFFFFFFFFF00ULL;
        uint64_t hvmask = discs & 0x007E7E7E7E7E7E00ULL;
        uint64_t res = 
            (hmask << 1) | (hmask >> 1) | 
            (vmask << HW) | (vmask >> HW) | 
            (hvmask << HW_M1) | (hvmask >> HW_M1) | 
            (hvmask << HW_P1) | (hvmask >> HW_P1);
        return pop_count_ull(empties & res);
    }
#endif

/*
    @brief Evaluate a move in midgame

    @param search               search information
    @param flip_value           flip with value
    @param alpha                alpha value to search
    @param beta                 beta value to search
    @param depth                depth to search
    @param searching            flag for terminating this search
    @return true if wipeout found else false
*/
inline void move_evaluate(Search *search, Flip_value *flip_value, int alpha, int beta, int depth, const bool *searching) {
    flip_value->value = 0;
    search->move(&flip_value->flip);
        flip_value->n_legal = search->board.get_legal();
        flip_value->value += (MO_OFFSET_L_PM - get_weighted_n_moves(flip_value->n_legal)) * W_MOBILITY;
        flip_value->value += (MO_OFFSET_L_PM - get_potential_mobility(search->board.opponent, ~(search->board.player | search->board.opponent))) * W_POTENTIAL_MOBILITY;
        switch (depth) {
            case 0:
                flip_value->value += (SCORE_MAX - mid_evaluate_diff(search)) * W_VALUE;
                break;
            case 1:
                flip_value->value += (SCORE_MAX - nega_alpha_eval1(search, alpha, beta, false)) * (W_VALUE + W_VALUE_DEEP_ADDITIONAL);
                break;
            default:
                if (transposition_table.has_node_any_level(search, search->board.hash())) {
                    flip_value->value += W_TT_BONUS;
                }
                uint_fast8_t mpc_level = search->mpc_level;
                search->mpc_level = MOVE_ORDERING_MPC_LEVEL;
                    flip_value->value += (SCORE_MAX - nega_scout(search, alpha, beta, depth, false, flip_value->n_legal, false, searching)) * (W_VALUE + depth * W_VALUE_DEEP_ADDITIONAL);
                search->mpc_level = mpc_level;
                break;
        }
    search->undo(&flip_value->flip);
}

/*
    @brief Evaluate a move in midgame for NWS

    @param search               search information
    @param flip_value           flip with value
    @param alpha                alpha value to search
    @param beta                 beta value to search
    @param depth                depth to search
    @param searching            flag for terminating this search
    @return true if wipeout found else false
*/
inline void move_evaluate_nws(Search *search, Flip_value *flip_value, int alpha, int beta, int depth, const bool *searching) {
    flip_value->value = 0;
    search->move(&flip_value->flip);
        flip_value->n_legal = search->board.get_legal();
        flip_value->value += (MO_OFFSET_L_PM - get_weighted_n_moves(flip_value->n_legal)) * W_NWS_MOBILITY;
        flip_value->value += (MO_OFFSET_L_PM - get_potential_mobility(search->board.opponent, ~(search->board.player | search->board.opponent))) * W_NWS_POTENTIAL_MOBILITY;
        switch (depth) {
            case 0:
                #if USE_LIGHT_EVALUATION
                    flip_value->value += (SCORE_MAX - mid_evaluate_light(search)) * W_NWS_VALUE;
                #else
                    flip_value->value += (SCORE_MAX - mid_evaluate_diff(search)) * W_NWS_VALUE;
                #endif
                break;
            case 1:
                #if USE_LIGHT_EVALUATION
                    flip_value->value += (SCORE_MAX - nega_alpha_light_eval1(search, alpha, beta, false)) * (W_NWS_VALUE + W_NWS_VALUE_DEEP_ADDITIONAL);
                #else
                    flip_value->value += (SCORE_MAX - nega_alpha_eval1(search, alpha, beta, false)) * (W_NWS_VALUE + W_NWS_VALUE_DEEP_ADDITIONAL);
                #endif
                break;
            default:
                if (transposition_table.has_node_any_level(search, search->board.hash())) {
                    flip_value->value += W_NWS_TT_BONUS;
                }
                uint_fast8_t mpc_level = search->mpc_level;
                search->mpc_level = MOVE_ORDERING_MPC_LEVEL;
                    flip_value->value += (SCORE_MAX - nega_scout(search, alpha, beta, depth, false, flip_value->n_legal, false, searching)) * (W_NWS_VALUE + depth * W_NWS_VALUE_DEEP_ADDITIONAL);
                search->mpc_level = mpc_level;
                break;
        }
    search->undo(&flip_value->flip);
}

/*
    @brief Evaluate a move in endgame NWS

    @param search               search information
    @param flip_value           flip with value
    @return true if wipeout found else false
*/
inline void move_evaluate_end_nws(Search *search, Flip_value *flip_value) {
    flip_value->value = 0;
    search->move_endsearch(&flip_value->flip);
        flip_value->n_legal = search->board.get_legal();
        flip_value->value += (MO_OFFSET_L_PM - get_n_moves_cornerX2(flip_value->n_legal)) * W_END_NWS_MOBILITY;
        flip_value->value += (MO_OFFSET_L_PM - mid_evaluate_move_ordering_end(search)) * W_END_NWS_VALUE;
    search->undo_endsearch(&flip_value->flip);
}

/*
    @brief Evaluate a move in endgame NWS (simple)

    @param search               search information
    @param flip_value           flip with value
    @return true if wipeout found else false
*/
inline void move_evaluate_end_simple_nws(Search *search, Flip_value *flip_value) {
    flip_value->value = 0;
    if (search->parity & cell_div4[flip_value->flip.pos]) {
        flip_value->value += W_END_NWS_SIMPLE_PARITY;
    }
    search->move_noeval(&flip_value->flip);
        flip_value->n_legal = search->board.get_legal();
        flip_value->value += (MO_OFFSET_L_PM - get_n_moves_cornerX2(flip_value->n_legal)) * W_END_NWS_SIMPLE_MOBILITY;
    search->undo_noeval(&flip_value->flip);
}

/*
    @brief Set the best move to the first element

    @param move_list            list of moves
    @param strt                 the first index
    @param siz                  the size of move_list
*/
inline void swap_next_best_move(std::vector<Flip_value> &move_list, const int strt, const int siz) {
    if (strt == siz - 1) {
        return;
    }
    int top_idx = strt;
    int best_value = move_list[strt].value;
    for (int i = strt + 1; i < siz; ++i) {
        if (best_value < move_list[i].value) {
            best_value = move_list[i].value;
            top_idx = i;
        }
    }
    if (top_idx != strt) {
        std::swap(move_list[strt], move_list[top_idx]);
    }
}

/*
    @brief Set the best move to the first element

    @param move_list            list of moves
    @param strt                 the first index
    @param siz                  the size of move_list
*/
inline void swap_next_best_move(Flip_value move_list[], const int strt, const int siz) {
    if (strt == siz - 1) {
        return;
    }
    int top_idx = strt;
    int best_value = move_list[strt].value;
    for (int i = strt + 1; i < siz; ++i) {
        if (best_value < move_list[i].value) {
            best_value = move_list[i].value;
            top_idx = i;
        }
    }
    if (top_idx != strt) {
        std::swap(move_list[strt], move_list[top_idx]);
    }
}

/*
    @brief Evaluate all legal moves for midgame

    @param search               search information
    @param move_list            list of moves
    @param moves                list of moves in transposition table
    @param depth                remaining depth
    @param alpha                alpha value
    @param beta                 beta value
    @param searching            flag for terminating this search
*/
inline void move_list_evaluate(Search *search, std::vector<Flip_value> &move_list, uint_fast8_t moves[], int depth, int alpha, int beta, const bool *searching) {
    if (move_list.size() == 1) {
        return;
    }
    int eval_alpha = -std::min(SCORE_MAX, beta + MOVE_ORDERING_VALUE_OFFSET_BETA);
    int eval_beta = -std::max(-SCORE_MAX, alpha - MOVE_ORDERING_VALUE_OFFSET_ALPHA);
    int eval_depth = depth >> 2;
    /*
    int l, u = SCORE_MAX;
    transposition_table.get_value_any_level(search, search->board.hash(), &l, &u);
    if (u <= alpha) {
        eval_depth = std::max(0, eval_depth - 4);
    }
    */
    for (Flip_value &flip_value: move_list) {
        #if USE_MID_ETC
            if (flip_value.flip.flip) {
                if (flip_value.flip.pos == moves[0]) {
                    flip_value.value = W_1ST_MOVE;
                } else if (flip_value.flip.pos == moves[1]) {
                    flip_value.value = W_2ND_MOVE;
                } else{
                    move_evaluate(search, &flip_value, eval_alpha, eval_beta, eval_depth, searching);
                }
            } else{
                flip_value.value = -INF;
            }
        #else
            if (flip_value.flip.pos == moves[0]) {
                flip_value.value = W_1ST_MOVE;
            } else if (flip_value.flip.pos == moves[1]) {
                flip_value.value = W_2ND_MOVE;
            } else{
                move_evaluate(search, &flip_value, eval_alpha, eval_beta, eval_depth, searching);
            }
        #endif
    }
}

/*
    @brief Evaluate all legal moves for midgame NWS

    @param search               search information
    @param move_list            list of moves
    @param moves                list of moves in transposition table
    @param depth                remaining depth
    @param alpha                alpha value (beta = alpha + 1)
    @param searching            flag for terminating this search
*/
inline void move_list_evaluate_nws(Search *search, std::vector<Flip_value> &move_list, uint_fast8_t moves[], int depth, int alpha, const bool *searching) {
    if (move_list.size() <= 1) {
        return;
    }
    const int eval_alpha = -std::min(SCORE_MAX, alpha + MOVE_ORDERING_NWS_VALUE_OFFSET_BETA);
    const int eval_beta = -std::max(-SCORE_MAX, alpha - MOVE_ORDERING_NWS_VALUE_OFFSET_ALPHA);
    int eval_depth = depth >> 4;
    for (Flip_value &flip_value: move_list) {
        #if USE_MID_ETC
            if (flip_value.flip.flip) {
                if (flip_value.flip.pos == moves[0]) {
                    flip_value.value = W_1ST_MOVE;
                } else if (flip_value.flip.pos == moves[1]) {
                    flip_value.value = W_2ND_MOVE;
                } else{
                    move_evaluate_nws(search, &flip_value, eval_alpha, eval_beta, eval_depth, searching);
                }
            } else{
                flip_value.value = -INF;
            }
        #else
            if (flip_value.flip.pos == moves[0]) {
                flip_value.value = W_1ST_MOVE;
            } else if (flip_value.flip.pos == moves[1]) {
                flip_value.value = W_2ND_MOVE;
            } else{
                move_evaluate_nws(search, &flip_value, eval_alpha, eval_beta, eval_depth, searching);
            }
        #endif
    }
}

/*
    @brief Evaluate all legal moves for endgame NWS

    @param search               search information
    @param move_list            list of moves
*/
inline void move_list_evaluate_end_nws(Search *search, std::vector<Flip_value> &move_list, uint_fast8_t moves[], const bool *searching) {
    if (move_list.size() <= 1) {
        return;
    }
    for (Flip_value &flip_value: move_list) {
        if (flip_value.flip.pos == moves[0]) {
            flip_value.value = W_1ST_MOVE;
        } else if (flip_value.flip.pos == moves[1]) {
            flip_value.value = W_2ND_MOVE;
        } else{
            move_evaluate_end_nws(search, &flip_value);
        }
    }
}

/*
    @brief Evaluate all legal moves for endgame NWS (simple)

    @param search               search information
    @param move_list            list of moves
*/
inline void move_list_evaluate_end_simple_nws(Search *search, Flip_value move_list[], const int canput) {
    if (canput <= 1) {
        return;
    }
    for (int i = 0; i < canput; ++i) {
        move_evaluate_end_simple_nws(search, &move_list[i]);
    }
}

inline void move_list_sort(std::vector<Flip_value> &move_list) {
    std::sort(move_list.begin(), move_list.end(), [](Flip_value &a, Flip_value &b) { return a.value > b.value; });
}

/*
    @brief Parameter tuning for move ordering
*/
#if TUNE_MOVE_ORDERING
    #include "ai.hpp"
    inline Search_result tree_search_legal(Board board, int depth, uint_fast8_t mpc_level, bool show_log, uint64_t use_legal, bool use_multi_thread);

    Board get_board(std::string board_str) {
        board_str.erase(std::remove_if(board_str.begin(), board_str.end(), ::isspace), board_str.end());
        Board new_board;
        int player = BLACK;
        new_board.player = 0ULL;
        new_board.opponent = 0ULL;
        if (board_str.length() != HW2 + 1) {
            std::cerr << "[ERROR] invalid argument" << std::endl;
            return new_board;
        }
        for (int i = 0; i < HW2; ++i) {
            if (board_str[i] == 'B' || board_str[i] == 'b' || board_str[i] == 'X' || board_str[i] == 'x' || board_str[i] == '0' || board_str[i] == '*')
                new_board.player |= 1ULL << (HW2_M1 - i);
            else if (board_str[i] == 'W' || board_str[i] == 'w' || board_str[i] == 'O' || board_str[i] == 'o' || board_str[i] == '1')
                new_board.opponent |= 1ULL << (HW2_M1 - i);
        }
        if (board_str[HW2] == 'B' || board_str[HW2] == 'b' || board_str[HW2] == 'X' || board_str[HW2] == 'x' || board_str[HW2] == '0' || board_str[HW2] == '*')
            player = BLACK;
        else if (board_str[HW2] == 'W' || board_str[HW2] == 'w' || board_str[HW2] == 'O' || board_str[HW2] == 'o' || board_str[HW2] == '1')
            player = WHITE;
        else{
            std::cerr << "[ERROR] invalid player argument" << std::endl;
            return new_board;
        }
        if (player == WHITE)
            std::swap(new_board.player, new_board.opponent);
        return new_board;
    }

    uint64_t n_nodes_test(int level, std::vector<Board> testcase_arr) {
        uint64_t n_nodes = 0;
        for (Board &board: testcase_arr) {
            int depth;
            bool is_mid_search;
            uint_fast8_t mpc_level;
            get_level(level, board.n_discs() - 4, &is_mid_search, &depth, &mpc_level);
            transposition_table.init();
            Search_result result = tree_search_legal(board, depth, mpc_level, false, board.get_legal(), true);
            n_nodes += result.nodes;
        }
        transposition_table.reset_importance();
        return n_nodes;
    }

    void tune_move_ordering(int level) {
        std::cout << "please input testcase file" << std::endl;
        std::string file;
        std::cin >> file;
        std::ifstream ifs(file);
        if (ifs.fail()) {
            std::cerr << "[ERROR] [FATAL] problem file " << file << " not found" << std::endl;
            return;
        }
        std::vector<Board> testcase_arr;
        std::string line;
        while (std::getline(ifs, line)) {
            testcase_arr.emplace_back(get_board(line));
        }
        std::cerr << testcase_arr.size() << " testcases loaded" << std::endl;
        int minute = 10;
        std::cout << "please input timelimit (minute)" << std::endl;
        std::cin >> minute;
        uint64_t tl = 60ULL * 1000ULL * minute; // 10 min
        uint64_t min_n_nodes = n_nodes_test(level, testcase_arr);
        double min_percentage = 100.0;
        uint64_t first_n_nodes = min_n_nodes;
        std::cerr << "min_n_nodes " << min_n_nodes << std::endl;
        int n_updated = 0;
        int n_try = 0;
        uint64_t strt = tim();
        while (tim() - strt < tl) {
            // update parameter randomly
            int idx = myrandrange(MOVE_ORDERING_PARAM_START, MOVE_ORDERING_PARAM_END + 1); // midgame search
            int delta = myrandrange(-5, 6);
            while (delta == 0)
                delta = myrandrange(-5, 6);
            if (move_ordering_param_array[idx] + delta < 0)
                continue;
            move_ordering_param_array[idx] += delta;
            uint64_t n_nodes = n_nodes_test(level, testcase_arr);
            double percentage = 100.0 * n_nodes / first_n_nodes;

            // simulated annealing
            constexpr double start_temp = 0.1; // percent
            constexpr double end_temp = 0.0001; // percent
            double temp = start_temp + (end_temp - start_temp) * (tim() - strt) / tl;
            double prob = exp((min_percentage - percentage) / temp);
            if (prob > myrandom()) {
                min_n_nodes = n_nodes;
                min_percentage = percentage;
                ++n_updated;
            } else{
                move_ordering_param_array[idx] -= delta;
            }
            ++n_try;

            std::cerr << "try " << n_try << " updated " << n_updated << " min_n_nodes " << min_n_nodes << " n_nodes " << n_nodes << " " << min_percentage << "% " << tim() - strt << " ms ";
            for (int i = 0; i < N_MOVE_ORDERING_PARAM; ++i) {
                std::cerr << ", " << move_ordering_param_array[i];
            }
            std::cerr << std::endl;
        }
        std::cout << "done " << min_percentage << "% ";
        for (int i = 0; i < N_MOVE_ORDERING_PARAM; ++i) {
            std::cout << ", " << move_ordering_param_array[i];
        }
        std::cout << std::endl;
    }
#endif
