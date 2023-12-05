﻿/*
    Egaroucid Project

    @file book.hpp
        Book class
    @date 2021-2023
    @author Takuto Yamana
    @license GPL-3.0 license
*/

#pragma once
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "evaluate.hpp"
#include "board.hpp"
#include "search.hpp"
#include "ai.hpp"

inline Search_result tree_search(Board board, int depth, uint_fast8_t mpc_level, bool show_log, bool use_multi_thread);
Search_result ai(Board board, int level, bool use_book, int book_acc_level, bool use_multi_thread, bool show_log);

#define BOOK_N_ACCEPT_LEVEL 11
#define BOOK_ACCURACY_LEVEL_INF 10
#define BOOK_LEAF_LEVEL 5

#define LEVEL_UNDEFINED -1
#define LEVEL_HUMAN 70
#define BOOK_LOSS_IGNORE_THRESHOLD 8

#define FORCE_BOOK_LEVEL false
#define FORCE_BOOK_DEPTH false

/*
    @brief book result structure

    @param policy               selected best move
    @param value                registered score
*/
struct Book_value{
    int policy;
    int value;

    Search_result to_search_result(){
        Search_result res;
        res.policy = policy;
        res.value = value;
        res.depth = SEARCH_BOOK;
        res.time = 0;
        res.nodes = 0;
        res.clog_time = 0;
        res.clog_nodes = 0;
        res.nps = 0;
        res.is_end_search - false;
        res.probability = -1;
        return res;
    }
};

struct Leaf{
    int8_t value;
    int8_t move;

    Leaf(){
        value = SCORE_UNDEFINED;
        move = -1;
    }
};

/*
    @brief book element structure

    @param value                registered score
    @param level                AI level
    @param moves                each moves and values
*/
struct Book_elem{
    int8_t value;
    Leaf leaf;

    Book_elem(){
        value = SCORE_UNDEFINED;
    }
};

/*
    @brief array for calculating hash code for book
*/
size_t hash_rand_player_book[4][65536];
size_t hash_rand_opponent_book[4][65536];

/*
    @brief initialize hash array for book randomly
*/
void book_hash_init_rand(){
    int i, j;
    for (i = 0; i < 4; ++i){
        for (j = 0; j < 65536; ++j){
            hash_rand_player_book[i][j] = 0;
            while (pop_count_uint(hash_rand_player_book[i][j]) < 9)
                hash_rand_player_book[i][j] = myrand_ull();
            hash_rand_opponent_book[i][j] = 0;
            while (pop_count_uint(hash_rand_opponent_book[i][j]) < 9)
                hash_rand_opponent_book[i][j] = myrand_ull();
        }
    }
}

/*
    @brief initialize hash array for book
*/
void book_hash_init(bool show_log){
    FILE* fp;
    if (!file_open(&fp, "resources/hash_book.eghs", "rb")){
        std::cerr << "[ERROR] can't open hash_book.eghs" << std::endl;
        book_hash_init_rand();
        return;
    }
    for (int i = 0; i < 4; ++i){
        if (fread(hash_rand_player_book[i], 8, 65536, fp) < 65536){
            std::cerr << "[ERROR] hash_book.eghs broken" << std::endl;
            book_hash_init_rand();
            return;
        }
    }
    for (int i = 0; i < 4; ++i){
        if (fread(hash_rand_opponent_book[i], 8, 65536, fp) < 65536){
            std::cerr << "[ERROR] hash_book.eghs broken" << std::endl;
            book_hash_init_rand();
            return;
        }
    }
    if (show_log)
        std::cerr << "hash for book initialized" << std::endl;
    return;
}

/*
    @brief Hash function for book

    @param board                board
    @return hash code
*/
struct Book_hash {
    size_t operator()(Board board) const{
        const uint16_t *p = (uint16_t*)&board.player;
        const uint16_t *o = (uint16_t*)&board.opponent;
        return 
            hash_rand_player_book[0][p[0]] ^ 
            hash_rand_player_book[1][p[1]] ^ 
            hash_rand_player_book[2][p[2]] ^ 
            hash_rand_player_book[3][p[3]] ^ 
            hash_rand_opponent_book[0][o[0]] ^ 
            hash_rand_opponent_book[1][o[1]] ^ 
            hash_rand_opponent_book[2][o[2]] ^ 
            hash_rand_opponent_book[3][o[3]];
    }
};







/*
    @brief book data

    @param book                 book data
*/
class Book{
    private:
        std::mutex mtx;
        std::unordered_map<Board, Book_elem, Book_hash> book;

    public:
        /*
            @brief initialize book

            @param file                 book file (.egbk file)
            @return book completely imported?
        */
        bool init(std::string file, bool show_log, bool *stop_loading){
            delete_all();
            if (!import_file_egbk3(file, show_log, stop_loading)){ // try egbk3 format
                if (!import_file_egbk2(file, show_log, stop_loading)){ // try egbk2 format
                    return import_file_egbk(file, show_log, stop_loading); // try egbk format
                }
            }
            return true;
        }

        /*
            @brief import Egaroucid-formatted book

            @param file                 book file (.egbk3 file)
            @return book completely imported?
        */
        inline bool import_file_egbk3(std::string file, bool show_log, bool *stop_loading){
            if (show_log)
                std::cerr << "importing " << file << std::endl;
            FILE* fp;
            if (!file_open(&fp, file.c_str(), "rb")){
                std::cerr << "[ERROR] can't open Egaroucid book " << file << std::endl;
                return false;
            }
            Board board;
            Book_elem book_elem;
            int n_boards;
            char value, leaf_value, leaf_move;
            uint64_t p, o;
            char egaroucid_str[10];
            char egaroucid_str_ans[] = "DICUORAGE";
            char elem_char;
            char book_version;
            // Header
            if (fread(egaroucid_str, 1, 9, fp) < 9) {
                std::cerr << "[ERROR] file broken" << std::endl;
                fclose(fp);
                return false;
            }
            for (int i = 0; i < 9; ++i){
                if (egaroucid_str[i] != egaroucid_str_ans[i]){
                    std::cerr << "[ERROR] This is not Egarocuid book, found " << egaroucid_str[i] << ", " << (int)egaroucid_str[i] << " at char " << i << ", expected " << egaroucid_str_ans[i] << " , " << (int)egaroucid_str_ans[i] << std::endl;
                    fclose(fp);
                    return false;
                }
            }
            if (fread(&book_version, 1, 1, fp) < 1) {
                std::cerr << "[ERROR] file broken" << std::endl;
                fclose(fp);
                return false;
            }
            if (book_version != 3){
                std::cerr << "[ERROR] This is not Egarocuid book version 3, found version" << (int)book_version << std::endl;
                fclose(fp);
                return false;
            }
            // Book Information
            if (fread(&n_boards, 4, 1, fp) < 1){
                std::cerr << "[ERROR] book broken at n_book data" << std::endl;
                fclose(fp);
                return false;
            }
            if (show_log)
                std::cerr << n_boards << " boards to read" << std::endl;
            // for each board
            int percent = -1;
            for (int i = 0; i < n_boards; ++i) {
                if (*stop_loading)
                    break;
                if (100 * i / n_boards > percent && show_log){
                    percent = 100 * i / n_boards;
                    std::cerr << "loading book " << percent << "%" << std::endl;
                }
                // read board, player
                if (fread(&p, 8, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read board, opponent
                if (fread(&o, 8, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // board error check
                if (p & o){
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read value
                if (fread(&value, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                if (value < -HW2 || HW2 < value) {
                    std::cerr << "[ERROR] book NOT FULLY imported got value " << value << " " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read leaf value
                if (fread(&leaf_value, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read leaf move
                if (fread(&leaf_move, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // push elem
                board.player = p;
                board.opponent = o;
                #if FORCE_BOOK_DEPTH
                    if (board.n_discs() <= 4 + 30){
                #endif
                        book_elem.value = value;
                        book_elem.leaf.value = leaf_value;
                        book_elem.leaf.move = leaf_move;
                        merge(board, book_elem);
                #if FORCE_BOOK_DEPTH
                    }
                #endif
            }
            if (*stop_loading){
                std::cerr << "stop loading book" << std::endl;
                fclose(fp);
                return false;
            }
            if (show_log)
                std::cerr << "imported " << book.size() << " boards to book" << std::endl;
            fclose(fp);
            return true;
        }

        inline bool import_file_bin_egbk3(std::string file, bool show_log){
            bool stop_loading = false;
            return import_file_bin_egbk3(file, show_log, &stop_loading);
        }


        void add_link(Board *board, int8_t value, int8_t policy){
            int rotate_idx;
            Board representive_board = get_representative_board(board, &rotate_idx);
            int8_t rotated_policy = convert_coord_from_representative_board((int)policy, rotate_idx);
            Leaf leaf;
            leaf.value = value;
            leaf.move = rotated_policy;
            book[representive_board].leaf = leaf;
        }

        void add_all_leaf(bool *stop){
            std::vector<Board> boards;
            for (auto itr = book.begin(); itr != book.end(); ++itr)
                boards.emplace_back(itr->first);
            Search search;
            bool searching = true;
            for (Board &board: boards){
                if (*stop)
                    break;
                if (book[board].leaf.value != SCORE_UNDEFINED)
                    continue;
                uint64_t legal = board.get_legal();
                uint64_t legal_loop = legal;
                Flip flip;
                for (uint_fast8_t cell = first_bit(&legal_loop); legal_loop; cell = next_bit(&legal_loop)){
                    calc_flip(&flip, &board, cell);
                    board.move_board(&flip);
                        int sgn = -1;
                        if (board.get_legal() == 0ULL){
                            sgn = 1;
                            board.pass();
                        }
                        if (contain(board)){
                            legal ^= 1ULL << cell;
                        }
                        if (sgn == 1)
                            board.pass();
                    board.undo_board(&flip);
                }
                if (legal){
                    int leaf_val = -65;
                    int leaf_move = MOVE_UNDEFINED;
                    for (uint_fast8_t cell = first_bit(&legal); legal; cell = next_bit(&legal)){
                        calc_flip(&flip, &board, cell);
                        board.move_board(&flip);
                            search.n_nodes = 0;
                            search.init_board(&board);
                            calc_features(&search);
                            int g = -nega_alpha_eval1(&search, -HW2, HW2, false, &searching);
                        board.undo_board(&flip);
                        if (leaf_val < g){
                            leaf_val = g;
                            leaf_move = cell;
                        }
                    }
                    add_link(&board, leaf_val, leaf_move);
                } else{
                    add_link(&board, SCORE_UNDEFINED, MOVE_UNDEFINED);
                }
            }
            
        }


        /*
            @brief import Egaroucid-formatted book (old)

            @param file                 book file (.egbk2 file)
            @return book completely imported?
        */
        inline bool import_file_egbk2(std::string file, bool show_log, bool *stop_loading){
            if (show_log)
                std::cerr << "importing " << file << std::endl;
            FILE* fp;
            if (!file_open(&fp, file.c_str(), "rb")){
                std::cerr << "[ERROR] can't open Egaroucid book " << file << std::endl;
                return false;
            }
            Board board;
            Book_elem book_elem;
            int n_boards;
            char value;
            uint64_t p, o;
            char level, n_moves, val, mov;
            char egaroucid_str[10];
            char egaroucid_str_ans[] = "DICUORAGE";
            char book_version;
            // Header
            if (fread(egaroucid_str, 1, 9, fp) < 9) {
                std::cerr << "[ERROR] file broken" << std::endl;
                fclose(fp);
                return false;
            }
            for (int i = 0; i < 9; ++i){
                if (egaroucid_str[i] != egaroucid_str_ans[i]){
                    std::cerr << "[ERROR] This is not Egarocuid book version 2, found " << egaroucid_str[i] << ", " << (int)egaroucid_str[i] << " at char " << i << ", expected " << egaroucid_str_ans[i] << " , " << (int)egaroucid_str_ans[i] << std::endl;
                    fclose(fp);
                    return false;
                }
            }
            if (fread(&book_version, 1, 1, fp) < 1) {
                std::cerr << "[ERROR] file broken" << std::endl;
                fclose(fp);
                return false;
            }
            if (book_version != 2){
                std::cerr << "[ERROR] This is not Egarocuid book version 2, found version" << (int)book_version << std::endl;
                fclose(fp);
                return false;
            }
            // Book Information
            if (fread(&n_boards, 4, 1, fp) < 1){
                std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                fclose(fp);
                return false;
            }
            if (show_log)
                std::cerr << n_boards << " boards to read" << std::endl;
            // for each board
            int percent = -1;
            for (int i = 0; i < n_boards; ++i) {
                if (*stop_loading)
                    break;
                if (100 * i / n_boards > percent && show_log){
                    percent = 100 * i / n_boards;
                    std::cerr << "loading book " << percent << "%" << std::endl;
                }
                // read board player
                if (fread(&p, 8, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read board opponent
                if (fread(&o, 8, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read value
                if (fread(&value, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                if (value < -HW2 || HW2 < value) {
                    std::cerr << "[ERROR] book NOT FULLY imported got value " << value << " " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read level (ignore this in egbk3)
                if (fread(&level, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read n_links
                if (fread(&n_moves, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // for each link
                for (uint8_t i = 0; i < n_moves; ++i){
                    // read value
                    if (fread(&val, 1, 1, fp) < 1) {
                        std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                        fclose(fp);
                        return false;
                    }
                    // read move
                    if (fread(&mov, 1, 1, fp) < 1) {
                        std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                        fclose(fp);
                        return false;
                    }
                    // ignore links...
                }
                board.player = p;
                board.opponent = o;
                #if FORCE_BOOK_DEPTH
                    if (b.n_discs() <= 4 + 30){
                #endif
                        book_elem.value = value;
                        book_elem.leaf.value = SCORE_UNDEFINED;
                        book_elem.leaf.move = MOVE_UNDEFINED;
                        merge(board, book_elem);
                #if FORCE_BOOK_DEPTH
                    }
                #endif
            }
            add_all_leaf(stop_loading);
            if (*stop_loading){
                std::cerr << "stop loading book" << std::endl;
                fclose(fp);
                return false;
            }
            if (show_log)
                std::cerr << "imported " << book.size() << " boards to book" << std::endl;
            fclose(fp);
            return true;
        }

        inline bool import_file_egbk2(std::string file, bool show_log){
            bool stop_loading = false;
            return import_file_egbk2(file, show_log, &stop_loading);
        }



        /*
            @brief import Egaroucid-formatted book (old)

            @param file                 book file (.egbk file)
            @return book completely imported?
        */
        inline bool import_file_egbk(std::string file, bool show_log, bool *stop_loading){
            if (show_log)
                std::cerr << "importing " << file << std::endl;
            FILE* fp;
            if (!file_open(&fp, file.c_str(), "rb")){
                std::cerr << "[ERROR] can't open Egaroucid book " << file << std::endl;
                return false;
            }
            Board board;
            Book_elem book_elem;
            int n_boards;
            char value;
            uint64_t p, o;
            uint8_t value_raw;
            // Book Information
            if (fread(&n_boards, 4, 1, fp) < 1){
                std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                fclose(fp);
                return false;
            }
            if (show_log)
                std::cerr << n_boards << " boards to read" << std::endl;
            // for each board
            int percent = -1;
            for (int i = 0; i < n_boards; ++i) {
                if (*stop_loading)
                    break;
                if (100 * i / n_boards > percent && show_log){
                    percent = 100 * i / n_boards;
                    std::cerr << "loading book " << percent << "%" << std::endl;
                }
                // read board player
                if (fread(&p, 8, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read board opponent
                if (fread(&o, 8, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read value
                if (fread(&value_raw, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] book NOT FULLY imported " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                value = (int8_t)value_raw - HW2;
                if (value < -HW2 || HW2 < value) {
                    std::cerr << "[ERROR] book NOT FULLY imported got value " << value << " " << book.size() << " boards" << std::endl;
                    fclose(fp);
                    return false;
                }
                board.player = p;
                board.opponent = o;
                #if FORCE_BOOK_DEPTH
                    if (b.n_discs() <= 4 + 30){
                #endif
                        book_elem.value = value;
                        book_elem.leaf.value = SCORE_UNDEFINED;
                        book_elem.leaf.move = MOVE_UNDEFINED;
                        merge(board, book_elem);
                #if FORCE_BOOK_DEPTH
                    }
                #endif
            }
            add_all_leaf(stop_loading);
            if (*stop_loading){
                std::cerr << "stop loading book" << std::endl;
                fclose(fp);
                return false;
            }
            if (show_log)
                std::cerr << "imported " << book.size() << " boards to book" << std::endl;
            fclose(fp);
            return true;
        }

        inline bool import_file_egbk(std::string file, bool show_log){
            bool stop_loading = false;
            return import_file_egbk(file, show_log, &stop_loading);
        }

        /*
            @brief import Edax-formatted book

            @param file                 book file (.dat file)
            @return book completely imported?
        */
        inline bool import_file_edax(std::string file, bool show_log) {
            if (show_log)
                std::cerr << "importing " << file << std::endl;
            FILE* fp;
            if (!file_open(&fp, file.c_str(), "rb")){
                std::cerr << "[ERROR] can't open Edax book " << file << std::endl;
                return false;
            }
            char elem_char;
            int elem_int;
            int16_t elem_short;
            // Header
            for (int i = 0; i < 38; ++i){
                if (fread(&elem_char, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] file broken" << std::endl;
                    fclose(fp);
                    return false;
                }
            }
            // Book Information
            if (fread(&elem_int, 4, 1, fp) < 1) {
                std::cerr << "[ERROR] file broken" << std::endl;
                fclose(fp);
                return false;
            }
            int n_boards = elem_int;
            uint64_t player, opponent;
            int16_t value;
            char link = 0, link_value, link_move, level, leaf_value, leaf_move;
            Board board;
            Flip flip;
            Book_elem book_elem;
            int percent = -1;
            for (int i = 0; i < n_boards; ++i){
                if (100 * i / n_boards > percent && show_log){
                    percent = 100 * i / n_boards;
                    std::cerr << "loading book " << percent << "%" << std::endl;
                }
                // read board player
                if (fread(&player, 8, 1, fp) < 1) {
                    std::cerr << "[ERROR] file broken" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read board opponent
                if (fread(&opponent, 8, 1, fp) < 1) {
                    std::cerr << "[ERROR] file broken" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read additional data
                for (int j = 0; j < 4; ++j) {
                    if (fread(&elem_int, 4, 1, fp) < 1) {
                        std::cerr << "[ERROR] file broken" << std::endl;
                        fclose(fp);
                        return false;
                    }
                }
                // read value
                if (fread(&value, 2, 1, fp) < 1) {
                    std::cerr << "[ERROR] file broken" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read additional data
                for (int j = 0; j < 2; ++j) {
                    if (fread(&elem_short, 2, 1, fp) < 1) {
                        std::cerr << "[ERROR] file broken" << std::endl;
                        fclose(fp);
                        return false;
                    }
                }
                // read link data
                if (fread(&link, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] file broken" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read level (ignore)
                if (fread(&level, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] file broken" << std::endl;
                    fclose(fp);
                    return false;
                }
                // for each link (ignore)
                for (int j = 0; j < (int)link; ++j) {
                    if (fread(&link_value, 1, 1, fp) < 1) {
                        std::cerr << "[ERROR] file broken" << std::endl;
                        fclose(fp);
                        return false;
                    }
                    if (fread(&link_move, 1, 1, fp) < 1) {
                        std::cerr << "[ERROR] file broken" << std::endl;
                        fclose(fp);
                        return false;
                    }
                }
                // read leaf value
                if (fread(&leaf_value, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] file broken" << std::endl;
                    fclose(fp);
                    return false;
                }
                // read leaf move
                if (fread(&leaf_move, 1, 1, fp) < 1) {
                    std::cerr << "[ERROR] file broken" << std::endl;
                    fclose(fp);
                    return false;
                }
                if (leaf_value < -HW2 || HW2 < leaf_value || leaf_move < 0 || HW2 <= leaf_move){
                    leaf_value = SCORE_UNDEFINED;
                    leaf_move = MOVE_UNDEFINED;
                }
                board.player = player;
                board.opponent = opponent;
                book_elem.value = value;
                book_elem.leaf.value = leaf_value;
                book_elem.leaf.move = leaf_move;
                merge(board, book_elem);
            }
            if (show_log)
                std::cerr << "imported " << book.size() << " boards to book" << std::endl;
            return true;
        }

        /*
            @brief save as Egaroucid-formatted book (.egbk3)

            @param file                 file name to save
            @param bak_file             backup file name
        */
        inline void save_egbk3(std::string file, std::string bak_file){
            if (remove(bak_file.c_str()) == -1)
                std::cerr << "cannot delete backup. you can ignore this error." << std::endl;
            rename(file.c_str(), bak_file.c_str());
            std::ofstream fout;
            fout.open(file.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
            if (!fout){
                std::cerr << "can't open " << file << std::endl;
                return;
            }
            char elem;
            std::cerr << "saving book..." << std::endl;
            char egaroucid_str[] = "DICUORAGE";
            fout.write((char*)&egaroucid_str, 9);
            char book_version = 3;
            fout.write((char*)&book_version, 1);
            int n_book = (int)book.size();
            fout.write((char*)&n_book, 4);
            int t = 0;
            for (auto itr = book.begin(); itr != book.end(); ++itr){
                ++t;
                if (t % 65536 == 0)
                    std::cerr << "saving book " << (t * 100 / (int)book.size()) << "%" << std::endl;
                fout.write((char*)&itr->first.player, 8);
                fout.write((char*)&itr->first.opponent, 8);
                elem = (char)itr->second.value;
                fout.write((char*)&elem, 1);
                elem = (char)itr->second.leaf.value;
                fout.write((char*)&elem, 1);
                elem = (char)itr->second.leaf.move;
                fout.write((char*)&elem, 1);
            }
            fout.close();
            int book_size = (int)book.size();
            std::cerr << "saved " << t << " boards , book_size " << book_size << std::endl;
        }

        /*
            @brief save as Edax-formatted book (.dat)

            @param file                 file name to save
            @param bak_file             backup file name
        */
        inline void save_bin_edax(std::string file){
            std::ofstream fout;
            fout.open(file.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
            if (!fout){
                std::cerr << "can't open " << file << std::endl;
                return;
            }
            std::cerr << "saving book..." << std::endl;
            char header[] = "XADEKOOB";
            for (int i = 0; i < 8; ++i)
                fout.write((char*)&header[i], 1);
            char ver = 4;
            fout.write((char*)&ver, 1);
            char rel = 4;
            fout.write((char*)&rel, 1);
            int year, month, day, hour, minute, second;
            calc_date(&year, &month, &day, &hour, &minute, &second);
            fout.write((char*)&year, 2);
            fout.write((char*)&month, 1);
            fout.write((char*)&day, 1);
            fout.write((char*)&hour, 1);
            fout.write((char*)&minute, 1);
            fout.write((char*)&second, 1);
            char dummy = 0;
            fout.write((char*)&dummy, 1);
            int level = 21; // fixed
            fout.write((char*)&level, 4);
            int n_empties = HW2;
            for (auto itr = book.begin(); itr != book.end(); ++itr)
                n_empties = std::min(n_empties, HW2 - itr->first.n_discs());
            fout.write((char*)&n_empties, 4);
            int err_mid = 0;
            fout.write((char*)&err_mid, 4);
            int err_end = 0;
            fout.write((char*)&err_end, 4);
            int verb = 0;
            fout.write((char*)&verb, 4);
            int n_position = book.size();
            fout.write((char*)&n_position, 4);
            uint64_t t = 0, n_leaf_add = 0;
            int n_win = 0, n_draw = 0, n_lose = 0;
            int n_line;
            short short_val, short_val_min = -HW2, short_val_max = HW2;
            char char_level;
            Book_elem book_elem;
            char link_value, link_move;
            int max_link_value, min_link_value;
            char leaf_val, leaf_move;
            char n_link;
            Flip flip;
            Board b;
            bool searching = true;
            int percent = -1;
            for (auto itr = book.begin(); itr != book.end(); ++itr){
                book_elem = itr->second;
                if (100 * t / book.size() > percent){
                    percent = 100 * t / book.size();
                    std::cerr << "converting book " << percent << "%" << std::endl;
                }
                ++t;
                short_val = book_elem.value;
                char_level = 21; // fixed
                std::vector<Book_value> links;
                b = itr->first;
                uint64_t legal = b.get_legal();
                Flip flip;
                for (uint_fast8_t cell = first_bit(&legal); legal; cell = next_bit(&legal)){
                    calc_flip(&flip, &b, cell);
                    b.move_board(&flip);
                        int sgn = -1;
                        if (b.get_legal() == 0ULL){
                            sgn = 1;
                            b.pass();
                        }
                        if (contain(b)){
                            Book_value book_value;
                            book_value.value = sgn * get(b).value;
                            book_value.policy = cell;
                            links.emplace_back(book_value);
                        }
                        if (sgn == 1)
                            b.pass();
                    b.undo_board(&flip);
                }
                n_link = (char)links.size();
                leaf_val = itr->second.leaf.value;
                leaf_move = itr->second.leaf.move;
                if (leaf_val == SCORE_UNDEFINED){
                    leaf_val = 0;
                    leaf_move = 65;
                }
                n_line = 0; //count_n_line(itr->first);
                fout.write((char*)&itr->first.player, 8);
                fout.write((char*)&itr->first.opponent, 8);
                fout.write((char*)&n_win, 4);
                fout.write((char*)&n_draw, 4);
                fout.write((char*)&n_lose, 4);
                fout.write((char*)&n_line, 4);
                fout.write((char*)&short_val, 2);
                fout.write((char*)&short_val_min, 2);
                fout.write((char*)&short_val_max, 2);
                fout.write((char*)&n_link, 1);
                fout.write((char*)&char_level, 1);
                for (Book_value &book_value: links){
                    link_value = (char)book_value.value;
                    link_move = (char)book_value.policy;
                    fout.write((char*)&link_value, 1);
                    fout.write((char*)&link_move, 1);
                }
                fout.write((char*)&leaf_val, 1);
                fout.write((char*)&leaf_move, 1);
            }
            fout.close();
            std::cerr << "saved " << t << " boards as a edax-formatted book " << n_position << " " << book.size() << " " << n_leaf_add << std::endl;
        }

        /*
            @brief register a board to book

            @param b                    a board to register
            @param elem                 book element
        */
        inline void reg(Board b, Book_elem elem){
            register_symmetric_book(b, elem);
        }

        /*
            @brief register a board to book

            @param b                    a board pointer to register
            @param elem                 book element
        */
        inline void reg(Board *b, Book_elem elem){
            register_symmetric_book(b->copy(), elem);
        }

        /*
            @brief check if book has a board

            @param b                    a board to find
            @return if contains, true, else false
        */
        inline bool contain_representative(Board b){
            return book.find(b) != book.end();
        }

        /*
            @brief check if book has a board

            @param b                    a board pointer to find
            @return if contains, true, else false
        */
        inline bool contain_representative(Board *b){
            return contain_representative(b->copy());
        }

        inline bool contain(Board b){
            return contain_representative(get_representative_board(b));
        }

        inline bool contain(Board *b){
            return contain_representative(get_representative_board(b));
        }

        /*
            @brief get registered score

            @param b                    a board to find
            @return registered value (if not registered, returns -INF)
        */
        inline Book_elem get_representative(Board b, int idx){
            Book_elem res;
            if (!contain_representative(b))
                return res;
            res = book[b];
            res.leaf.move = convert_coord_from_representative_board(res.leaf.move, idx);
            return res;
        }

        /*
            @brief get registered score with all rotation

            @param b                    a board pointer to find
            @return registered value (if not registered, returns -INF)
        */
        inline Book_elem get(Board *b){
            int rotate_idx;
            Board representive_board = get_representative_board(b, &rotate_idx);
            return get_representative(representive_board, rotate_idx);
        }

        /*
            @brief get registered score with all rotation

            @param b                    a board to find
            @return registered value (if not registered, returns -INF)
        */
        inline Book_elem get(Board b){
            int rotate_idx;
            Board representive_board = get_representative_board(b, &rotate_idx);
            return get_representative(representive_board, rotate_idx);
        }

        /*
            @brief get all best moves

            @param b                    a board pointer to find
            @return vector of best moves
        */
        inline std::vector<int> get_all_best_moves(Board *b){
            std::vector<int> policies;
            uint64_t legal = b->get_legal();
            int max_value = -INF;
            Flip flip;
            for (uint_fast8_t cell = first_bit(&legal); legal; cell = next_bit(&legal)){
                calc_flip(&flip, b, cell);
                b->move_board(&flip);
                    int sgn = -1;
                    if (b->get_legal() == 0ULL){
                        sgn = 1;
                        b->pass();
                    }
                    if (contain(b)){
                        Book_elem elem = get(b);
                        if (sgn * elem.value > max_value){
                            max_value = sgn * elem.value;
                            policies.clear();
                        }
                        if (sgn * elem.value == max_value)
                            policies.emplace_back(cell);
                    }
                    if (sgn == 1)
                        b->pass();
                b->undo_board(&flip);
            }
            return policies;
        }

        /*
            @brief get all registered moves with value

            @param b                    a board pointer to find
            @return vector of moves
        */
        inline std::vector<Book_value> get_all_moves_with_value(Board *b){
            std::vector<Book_value> policies;
            uint64_t legal = b->get_legal();
            Flip flip;
            for (uint_fast8_t cell = first_bit(&legal); legal; cell = next_bit(&legal)){
                calc_flip(&flip, b, cell);
                b->move_board(&flip);
                    int sgn = -1;
                    if (b->get_legal() == 0ULL){
                        sgn = 1;
                        b->pass();
                    }
                    if (contain(b)){
                        Book_value book_value;
                        book_value.policy = cell;
                        book_value.value = sgn * get(b).value;
                        policies.emplace_back(book_value);
                    }
                    if (sgn == 1)
                        b->pass();
                b->undo_board(&flip);
            }
            return policies;
        }

        /*
            @brief get a best move

            @param b                    a board pointer to find
            @param acc_level            accuracy level, 0 is very good, 10 is very bad
            @return best move and value as Book_value structure
        */
        inline Book_value get_random(Board *b, int acc_level){
            std::vector<std::pair<double, int>> value_policies;
            std::vector<std::pair<int, int>> value_policies_memo;
            uint64_t legal = b->get_legal();
            double best_score = -INF;
            Flip flip;
            for (uint_fast8_t cell = first_bit(&legal); legal; cell = next_bit(&legal)){
                calc_flip(&flip, b, cell);
                b->move_board(&flip);
                    int sgn = -1;
                    if (b->get_legal() == 0ULL){
                        sgn = 1;
                        b->pass();
                    }
                    if (contain(b)){
                        Book_value book_value;
                        book_value.policy = cell;
                        book_value.value = sgn * get(b).value;
                        if (book_value.value > best_score)
                            best_score = (double)book_value.value;
                        value_policies.emplace_back(std::make_pair((double)book_value.value, cell));
                        value_policies_memo.emplace_back(std::make_pair(book_value.value, cell));
                    }
                    if (sgn == 1)
                        b->pass();
                b->undo_board(&flip);
            }
            Book_elem board_elem = get(b);
            Book_value res;
            if (value_policies.size() == 0 || best_score < board_elem.value - BOOK_LOSS_IGNORE_THRESHOLD){
                res.policy = -1;
                res.value = -INF;
                return res;
            }
            double acceptable_min_value = best_score - 2.0 * acc_level - 0.5;
            double sum_exp_values = 0.0;
            for (std::pair<double, int> &elem: value_policies){
                if (elem.first < acceptable_min_value)
                    elem.first = 0.0;
                else{
                    double exp_val = (exp(elem.first - best_score) + 1.5) / 3.0;
                    elem.first = pow(exp_val, BOOK_ACCURACY_LEVEL_INF - acc_level);
                }
                sum_exp_values += elem.first;
            }
            for (std::pair<double, int> &elem: value_policies)
                elem.first /= sum_exp_values;
            double rnd = myrandom();
            double s = 0.0;
            bool res_got = false;
            for (std::pair<double, int> &elem: value_policies){
                s += elem.first;
                if (s >= rnd){
                    res.policy = elem.second;
                    for (std::pair<int, int> elem: value_policies_memo){
                        if (elem.second == res.policy)
                            res.value = elem.first;
                    }
                    res_got = true;
                    break;
                }
            }
            if (!res_got){
                res.policy = value_policies.back().second;
                for (std::pair<int, int> elem: value_policies_memo){
                    if (elem.second == res.policy)
                        res.value = elem.first;
                }
            }
            return res;
        }

        /*
            @brief get how many boards registered in this book

            @return number of registered boards
        */
        inline int get_n_book(){
            return (int)book.size();
        }

        /*
            @brief change or register a board

            @param b                    a board to change or register
            @param value                a value to change or register
        */
        inline void change(Board b, int value){
            if (-HW2 <= value && value <= HW2){
                if (contain(b)){
                    Board bb = get_representative_board(b);
                    book[bb].value = value;
                } else{
                    Book_elem elem;
                    elem.value = value;
                    register_symmetric_book(b, elem);
                }
            }
        }

        /*
            @brief change or register a board

            @param b                    a board pointer to change or register
            @param value                a value to change or register
        */
        inline void change(Board *b, int value){
            Board nb = b->copy();
            change(nb, value);
        }

        /*
            @brief delete a board

            @param b                    a board to delete
        */
        inline void delete_elem(Board b){
            if (delete_symmetric_book(b)){
                std::cerr << "deleted book elem " << book.size() << std::endl;
            } else
                std::cerr << "book elem NOT deleted " << book.size() << std::endl;
        }

        /*
            @brief delete all board in this book
        */
        inline void delete_all(){
            book.clear();
            reg_first_board();
        }

        /*
            @brief fix book
        */
        inline void fix(bool *stop){
            link_book(stop);
            //delete_isolated_node(stop);
            //negamax_book_delete_suspicious(stop);
            negamax_book(stop);
        }

        /*
            @brief fix book
        */
        inline void fix(){
            bool stop = false;
            fix(&stop);
        }

        void link_book(bool *stop){
            std::cerr << "linking book..." << std::endl;
            std::vector<Board> boards;
            for (auto itr = book.begin(); itr != book.end(); ++itr)
                boards.emplace_back(itr->first);
            Book_elem book_elem;
            Flip flip;
            int t = 0;
            Board nb;
            uint64_t legal;
            bool elem_changed;
            for (Board &b: boards){
                if (*stop || !global_searching)
                    break;
                ++t;
                if (t % 16384 == 0)
                    std::cerr << "linking book " << (t * 100 / (int)boards.size()) << "%" << std::endl;
                book_elem = book[b];
                legal = b.get_legal();
                for (Book_value &elem: book_elem.moves)
                    legal ^= 1ULL << elem.policy;
                if (legal){
                    elem_changed = false;
                    for (uint_fast8_t cell = first_bit(&legal); legal; cell = next_bit(&legal)){
                        calc_flip(&flip, &b, cell);
                        b.move_board(&flip);
                            if (b.get_legal()){
                                if (contain(b)){
                                    Book_value move;
                                    move.policy = cell;
                                    move.value = -get(b).value;
                                    book_elem.moves.emplace_back(move);
                                    elem_changed = true;
                                }
                            } else{
                                b.pass();
                                    if (contain(b)){
                                        Book_value move;
                                        move.policy = cell;
                                        move.value = get(b).value;
                                        book_elem.moves.emplace_back(move);
                                        elem_changed = true;
                                    }
                                b.pass();
                            }
                        b.undo_board(&flip);
                    }
                    if (elem_changed)
                        book[b] = book_elem;
                }
            }
        }

        void delete_isolated_node(bool *stop){
            std::cerr << "deleting isolated nodes..." << std::endl;
            std::unordered_set<Board, Book_hash> child_boards;
            Board root_board;
            root_board.reset();
            register_tree_node(root_board, child_boards, stop);
            std::vector<Board> boards;
            for (auto itr = book.begin(); itr != book.end(); ++itr)
                boards.emplace_back(itr->first);
            uint64_t n_delete = 0;
            for (Board &board: boards){
                if (*stop)
                    break;
                if (child_boards.find(board) == child_boards.end()){
                    delete_representative_board(board);
                    ++n_delete;
                }
            }
            std::cerr << "deleted " << n_delete << " isolated nodes " << std::endl;
        }

        void register_tree_node(Board board, std::unordered_set<Board, Book_hash> &child_boards, const bool *stop){
            if (*stop)
                return;
            Board rep_board = get_representative_board(board);
            if (child_boards.find(rep_board) != child_boards.end())
                return;
            child_boards.emplace(rep_board);
            if (contain(&board)){
                size_t siz = child_boards.size();
                std::cerr << "connected book size " << siz << std::endl;
                Book_elem book_elem = get(board);
                Flip flip;
                for (Book_value &elem: book_elem.moves){
                    calc_flip(&flip, &board, elem.policy);
                    board.move_board(&flip);
                        register_tree_node(board, child_boards, stop);
                    board.undo_board(&flip);
                }
            }
        }

        void get_need_to_change_tasks(std::vector<std::pair<Board, Book_elem>> &root_boards, int *root_board_n_discs){
            for (auto itr = book.begin(); itr != book.end(); ++itr){
                if (itr->first.n_discs() >= *root_board_n_discs && itr->second.moves.size()){
                    int max_value = -INF;
                    Board b;
                    b.player = itr->first.player;
                    b.opponent = itr->first.opponent;
                    Flip flip;
                    bool is_leaf;
                    std::vector<Book_value> new_moves;
                    bool update_child_value = false;
                    for (const Book_value &elem: itr->second.moves){
                        is_leaf = true;
                        calc_flip(&flip, &b, elem.policy);
                        int child_value = SCORE_UNDEFINED;
                        b.move_board(&flip);
                            if (b.get_legal()){
                                if (contain(b))
                                    child_value = -get(b).value;
                            } else{
                                b.pass();
                                    if (contain(b))
                                        child_value = get(b).value;
                                b.pass();
                            }
                        b.undo_board(&flip);
                        if (child_value != SCORE_UNDEFINED){
                            max_value = std::max(max_value, child_value);
                            is_leaf = false;
                        }
                        Book_value child_book_value;
                        child_book_value.policy = elem.policy;
                        child_book_value.value = child_value;
                        new_moves.emplace_back(child_book_value);
                        update_child_value |= elem.value != child_value;
                    }
                    bool update_parent_value = max_value != itr->second.value && max_value != -INF;
                    if (update_parent_value || update_child_value){
                        Board root_board;
                        root_board.player = itr->first.player;
                        root_board.opponent = itr->first.opponent;
                        int n_discs = root_board.n_discs();
                        if (n_discs > *root_board_n_discs){
                            *root_board_n_discs = n_discs;
                            root_boards.clear();
                        }
                        Book_elem new_elem;
                        if (update_parent_value)
                            new_elem.value = max_value;
                        else
                            new_elem.value = itr->second.value;
                        new_elem.moves = new_moves; // always update
                        new_elem.level = itr->second.level;
                        root_boards.emplace_back(std::make_pair(root_board, new_elem));
                    }
                }
            }
        }

        void negamax_book(bool *stop){
            std::cerr << "negamaxing book..." << std::endl;
            std::vector<std::pair<Board, Book_elem>> root_boards;
            uint64_t n_fixed = 0;
            bool looped = true;
            while (looped){
                looped = false;
                int root_board_n_discs = 1;
                while (root_board_n_discs && !(*stop)){
                    root_boards.clear();
                    root_board_n_discs = 0;
                    get_need_to_change_tasks(root_boards, &root_board_n_discs);
                    if (root_board_n_discs){
                        for (std::pair<Board, Book_elem> &elem: root_boards){
                            Board bb = get_representative_board(elem.first);
                            //book[bb] = elem.second;
                            book.insert_or_assign(bb, elem.second);
                        }
                        n_fixed += root_boards.size();
                        looped = true;
                    }
                    std::cerr << "negamaxing book... fixed " << n_fixed << " boards" << std::endl;
                }
            }
            std::cerr << "negamaxed book with " << n_fixed << " fix" << std::endl;
        }

        void negamax_book_delete_suspicious(bool *stop){
            std::cerr << "negamaxing book..." << std::endl;
            std::vector<std::pair<Board, int>> root_boards;
            std::vector<Board> suspicious_boards;
            Book_elem book_elem;
            uint64_t n_fixed = 0;
            bool looped = true;
            while (looped){
                looped = false;
                int root_board_n_discs = 1;
                while (root_board_n_discs && !(*stop)){
                    root_boards.clear();
                    suspicious_boards.clear();
                    root_board_n_discs = 0;
                    for (auto itr = book.begin(); itr != book.end(); ++itr){
                        if (itr->first.n_discs() >= root_board_n_discs && itr->second.moves.size()){
                            book_elem = itr->second;
                            int max_value = -INF;
                            Board b;
                            b.player = itr->first.player;
                            b.opponent = itr->first.opponent;
                            Flip flip;
                            bool is_leaf;
                            for (Book_value &elem: book_elem.moves){
                                is_leaf = true;
                                calc_flip(&flip, &b, elem.policy);
                                b.move_board(&flip);
                                    if (b.get_legal()){
                                        if (contain(b)){
                                            max_value = std::max(max_value, -get(b).value);
                                            is_leaf = false;
                                        }
                                    } else{
                                        b.pass();
                                            if (contain(b)){
                                                max_value = std::max(max_value, get(b).value);
                                                is_leaf = false;
                                            }
                                        b.pass();
                                    }
                                b.undo_board(&flip);
                                //if (is_leaf)
                                //    max_value = std::max(max_value, elem.value);
                            }
                            if (max_value != book_elem.value && max_value != -INF){
                                Board root_board;
                                root_board.player = itr->first.player;
                                root_board.opponent = itr->first.opponent;
                                if (abs(max_value - book_elem.value) > 6){
                                    suspicious_boards.emplace_back(root_board);
                                } else{
                                    int n_discs = root_board.n_discs();
                                    if (n_discs > root_board_n_discs){
                                        root_board_n_discs = n_discs;
                                        root_boards.clear();
                                    }
                                    root_boards.emplace_back(std::make_pair(root_board, max_value));
                                }
                            }
                        }
                    }
                    if (suspicious_boards.size()){
                        for (Board &board: suspicious_boards){
                            delete_representative_board(board);
                        }
                        looped = true;
                    }
                    if (root_board_n_discs){
                        for (std::pair<Board, int> &elem: root_boards){
                            Board bb = get_representative_board(elem.first);
                            book[bb].value = elem.second;
                        }
                        n_fixed += root_boards.size();
                        looped = true;
                    }
                    std::cerr << "negamaxing book... fixed " << n_fixed << " boards" << std::endl;
                }
            }
            std::cerr << "negamaxed book with " << n_fixed << " fix" << std::endl;
        }

        void depth_align(int max_depth, bool *stop){
            std::vector<Board> boards;
            for (auto itr = book.begin(); itr != book.end(); ++itr)
                boards.emplace_back(itr->first);
            uint64_t t = 0;
            for (Board &board: boards){
                if (*stop)
                    break;
                ++t;
                if (t % 16384 == 0)
                    std::cerr << "cutting book " << (t * 100 / (int)boards.size()) << "%" << std::endl;
                if (board.n_discs() > 4 + max_depth){
                    book.erase(board);
                }
            }
            //fix(stop);
        }

        void rewrite_level(int level, bool *stop){
            uint64_t t = 0;
            for (auto itr = book.begin(); itr != book.end(); ++itr){
                if (*stop)
                    break;
                ++t;
                if (t % 16384 == 0)
                    std::cerr << "rewriting book level " << (t * 100 / (int)book.size()) << "%" << std::endl;
                itr->second.level = level;
            }
            //fix(stop);
        }

    private:
        void reg_first_board(){
            Board board;
            board.reset();
            Book_elem elem;
            elem.value = 0;
            elem.level = LEVEL_HUMAN;
            Book_value move;
            move.value = 0;
            move.policy = 19;
            elem.moves.emplace_back(move);
            move.policy = 26;
            elem.moves.emplace_back(move);
            move.policy = 37;
            elem.moves.emplace_back(move);
            move.policy = 44;
            elem.moves.emplace_back(move);
            book[board] = elem;
        }

        /*
            @brief register a board

            @param b                    a board to register
            @param value                score of the board
            @return is this board new?
        */
        inline bool register_representative(Board b, Book_elem elem){
            /*
            if (elem.value < -HW2 && HW2 < elem.value)
                return false;
            std::vector<Book_value> moves;
            for (Book_value &move: elem.moves){
                if (-HW2 <= move.value && move.value <= HW2)
                    moves.emplace_back(move);
            }
            elem.moves = moves;
            */
            int f_size = book.size();
            book[b] = elem;
            return book.size() - f_size > 0;
        }

        /*
            @brief delete a board

            @param b                    a board to delete
            @return board deleted?
        */
        inline bool delete_representative_board(Board b){
            if (book.find(b) != book.end()){
                book.erase(b);
                return true;
            }
            return false;
        }

        inline void first_update_representative_board(Board *res, Board *sym){
            uint64_t vp = vertical_mirror(sym->player);
            uint64_t vo = vertical_mirror(sym->opponent);
            if (res->player > vp || (res->player == vp && res->opponent > vo)){
                res->player = vp;
                res->opponent = vo;
            }
        }

        inline void update_representative_board(Board *res, Board *sym){
            if (res->player > sym->player || (res->player == sym->player && res->opponent > sym->opponent))
                sym->copy(res);
            uint64_t vp = vertical_mirror(sym->player);
            uint64_t vo = vertical_mirror(sym->opponent);
            if (res->player > vp || (res->player == vp && res->opponent > vo)){
                res->player = vp;
                res->opponent = vo;
            }
        }

        inline Board get_representative_board(Board b){
            Board res = b;
            first_update_representative_board(&res, &b);
            b.board_black_line_mirror();
            update_representative_board(&res, &b);
            b.board_horizontal_mirror();
            update_representative_board(&res, &b);
            b.board_white_line_mirror();
            update_representative_board(&res, &b);
            return res;
        }

        inline void first_update_representative_board(Board *res, Board *sym, int *idx, int *cnt){
            uint64_t vp = vertical_mirror(sym->player);
            uint64_t vo = vertical_mirror(sym->opponent);
            ++(*cnt);
            if (res->player > vp || (res->player == vp && res->opponent > vo)){
                res->player = vp;
                res->opponent = vo;
                *idx = *cnt;
            }
        }

        inline void update_representative_board(Board *res, Board *sym, int *idx, int *cnt){
            ++(*cnt);
            if (res->player > sym->player || (res->player == sym->player && res->opponent > sym->opponent)){
                sym->copy(res);
                *idx = *cnt;
            }
            uint64_t vp = vertical_mirror(sym->player);
            uint64_t vo = vertical_mirror(sym->opponent);
            ++(*cnt);
            if (res->player > vp || (res->player == vp && res->opponent > vo)){
                res->player = vp;
                res->opponent = vo;
                *idx = *cnt;
            }
        }

        inline Board get_representative_board(Board b, int *idx){
            Board res = b;
            *idx = 0;
            int cnt = 0;
            first_update_representative_board(&res, &b, idx, &cnt);
            b.board_black_line_mirror();
            update_representative_board(&res, &b, idx, &cnt);
            b.board_horizontal_mirror();
            update_representative_board(&res, &b, idx, &cnt);
            b.board_white_line_mirror();
            update_representative_board(&res, &b, idx, &cnt);
            return res;
        }

        inline Board get_representative_board(Board *b, int *idx){
            return get_representative_board(b->copy(), idx);
        }

        inline int convert_coord_from_representative_board(int cell, int idx){
            int res;
            int y = cell / HW;
            int x = cell % HW;
            switch (idx){
                case 0:
                    res = cell;
                    break;
                case 1:
                    res = (HW_M1 - y) * HW + x; // vertical
                    break;
                case 2:
                    res = (HW_M1 - x) * HW + (HW_M1 - y); // black line
                    break;
                case 3:
                    res = (HW_M1 - x) * HW + y; // black line + vertical ( = rotate 90 clockwise)
                    break;
                case 4:
                    res = x * HW + (HW_M1 - y); // black line + horizontal ( = rotate 90 counterclockwise)
                    break;
                case 5:
                    res = x * HW + y; // black line + horizontal + vertical ( = white line)
                    break;
                case 6:
                    res = y * HW + (HW_M1 - x); // horizontal
                    break;
                case 7:
                    res = (HW_M1 - y) * HW + (HW_M1 - x); // horizontal + vertical ( = rotate180)
                    break;
                default:
                    std::cerr << "converting coord error in book" << std::endl;
                    break;
            }
            return res;
        }

        inline int convert_coord_to_representative_board(int cell, int idx){
            int res;
            int y = cell / HW;
            int x = cell % HW;
            switch (idx){
                case 0:
                    res = cell;
                    break;
                case 1:
                    res = (HW_M1 - y) * HW + x; // vertical
                    break;
                case 2:
                    res = (HW_M1 - x) * HW + (HW_M1 - y); // black line
                    break;
                case 3:
                    res = x * HW + (HW_M1 - y); // black line + vertical ( = rotate 90 clockwise)
                    break;
                case 4:
                    res = (HW_M1 - x) * HW + y; // black line + horizontal ( = rotate 90 counterclockwise)
                    break;
                case 5:
                    res = x * HW + y; // black line + horizontal + vertical ( = white line)
                    break;
                case 6:
                    res = y * HW + (HW_M1 - x); // horizontal
                    break;
                case 7:
                    res = (HW_M1 - y) * HW + (HW_M1 - x); // horizontal + vertical ( = rotate180)
                    break;
                default:
                    std::cerr << "converting coord error in book" << std::endl;
                    break;
            }
            return res;
        }

        inline Board get_representative_board(Board *b){
            return get_representative_board(b->copy());
        }

        /*
            @brief register a board with checking all symmetry boards

            @param b                    a board to register
            @param value                score of the board
            @return 1 if board is new else 0
        */
        inline int register_symmetric_book(Board b, Book_elem elem){
            int idx;
            Board representive_board = get_representative_board(b, &idx);
            for (Book_value &move: elem.moves)
                move.policy = convert_coord_to_representative_board(move.policy, idx);
            return register_representative(representive_board, elem);
        }

        /*
            @brief delete a board with checking all symmetry boards

            @param b                    a board to delete
            @return 1 if board is deleted (board found) else 0
        */
        inline int delete_symmetric_book(Board b){
            Board representive_board = get_representative_board(b);
            return delete_representative_board(representive_board);
        }

        inline int merge(Board b, Book_elem elem){
            if (!contain(b))
                return register_symmetric_book(b, elem);
            Book_elem book_elem = get(b);
            book_elem.value = elem.value;
            book_elem.level = elem.level;
            for (Book_value &move: elem.moves){
                bool already_registered = false;
                for (int i = 0; i < (int)book_elem.moves.size(); ++i){
                    if (book_elem.moves[i].policy == move.policy){
                        already_registered = true;
                        book_elem.moves[i].value = move.value;
                        break;
                    }
                }
                if (!already_registered){
                    book_elem.moves.emplace_back(move);
                }
            }
            return register_symmetric_book(b, book_elem);
        }

        int count_n_line(Board board){
            auto itr = n_lines.find(board);
            if (itr != n_lines.end())
                return itr->second;
            Flip flip;
            Book_elem book_elem = get(board);
            if (book_elem.moves.size() == 0)
                return 1;
            int res = 0;
            for (Book_value &move: book_elem.moves){
                calc_flip(&flip, &board, move.policy);
                board.move_board(&flip);
                    if (contain(&board))
                        res += count_n_line(board);
                board.undo_board(&flip);
            }
            n_lines[board] = res;
            return res;
        }
};








Book_egbk2 book;

bool book_init(std::string file, bool show_log){
    //book_hash_init(show_log);
    book_hash_init_rand();
    bool stop_loading = false;
    return book.init(file, show_log, &stop_loading);
}

bool book_init(std::string file, bool show_log, bool *stop_loading){
    //book_hash_init(show_log);
    book_hash_init_rand();
    return book.init(file, show_log, stop_loading);
}

void book_save_as_edax(std::string file){
    book.save_bin_edax(file);
}

void book_fix(bool *stop){
    book.fix(stop);
}

void book_depth_align(int depth, bool *stop){
    book.depth_align(depth, stop);
}

void book_rewrite_level(int level, bool *stop){
    book.rewrite_level(level, stop);
}