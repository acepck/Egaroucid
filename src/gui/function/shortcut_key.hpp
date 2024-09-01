/*
    Egaroucid Project

    @file shortcut_key.hpp
        Shortcut Key Manager
    @date 2021-2024
    @author Takuto Yamana
    @license GPL-3.0 license
*/

#pragma once
#include <Siv3D.hpp>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "language.hpp"

#define SHORTCUT_KEY_UNDEFINED U"undefined"

struct Shortcut_key_elem{
    String name;
    std::vector<String> keys;
    std::vector<std::vector<std::string>> description_keys;
};

std::vector<Shortcut_key_elem> shortcut_keys_default = {
    // buttons
    {U"start_game",             {U"Space"},             {{"play", "start_game"}}},

    // game
    {U"new_game",               {U"Ctrl", U"N"},        {{"play", "game"}, {"play", "new_game"}}},
    {U"new_game_human_black",   {},                     {{"play", "game"}, {"play", "new_game_human_black"}}},
    {U"new_game_human_white",   {},                     {{"play", "game"}, {"play", "new_game_human_white"}}},
    {U"new_selfplay",           {},                     {{"play", "game"}, {"play", "new_selfplay"}}},
    {U"analyze",                {U"A"},                 {{"play", "game"}, {"play", "analyze"}}},

    // settings
    {U"ai_put_black",           {U"B"},                 {{"settings", "settings"}, {"settings", "play", "ai_put_black"}}},
    {U"ai_put_white",           {U"W"},                 {{"settings", "settings"}, {"settings", "play", "ai_put_white"}}},

    // display
    {U"disc_value",             {U"V"},                 {{"display", "display"}, {"display", "cell", "disc_value"}}},
    {U"umigame_value",          {U"U"},                 {{"display", "display"}, {"display", "cell", "umigame_value"}}},
    {U"graph_value",            {U"D"},                 {{"display", "display"}, {"display", "graph", "value"}}},
    {U"graph_sum_of_loss",      {U"S"},                 {{"display", "display"}, {"display", "graph", "sum_of_loss"}}},
    {U"laser_pointer",          {U"P"},                 {{"display", "display"}, {"display", "laser_pointer"}}},

    // operate
    {U"put_1_move_by_ai",       {U"G"},                 {{"operation", "operation"}, {"operation", "put_1_move_by_ai"}}},
    {U"forward",                {U"Right"},             {{"operation", "operation"}, {"operation", "forward"}}},
    {U"backward",               {U"Left"},              {{"operation", "operation"}, {"operation", "backward"}}},
    {U"undo",                   {U"Backspace"},         {{"operation", "operation"}, {"operation", "undo"}}},
    {U"save_this_branch",       {U"Ctrl", U"L"},        {{"operation", "operation"}, {"operation", "save_this_branch"}}},
    {U"generate_random_board",  {U"Ctrl", U"R"},        {{"operation", "operation"}, {"operation", "generate_random_board", "generate"}}},
    {U"stop_calculating",       {U"Q"},                 {{"operation", "operation"}, {"operation", "ai_operation", "stop_calculating"}}},

    // input / output
    {U"input_transcript",       {U"Ctrl", U"T"},        {{"in_out", "in_out"}, {"in_out", "input_transcript"}}},
    {U"edit_board",             {U"Ctrl", U"E"},        {{"in_out", "in_out"}, {"in_out", "edit_board"}}},
    {U"output_transcript",      {U"Ctrl", U"C"},        {{"in_out", "in_out"}, {"in_out", "output_transcript"}}},
    {U"screen_shot",            {U"Ctrl", U"S"},        {{"in_out", "in_out"}, {"in_out", "screen_shot"}}},
};

String generate_key_str(std::vector<String> keys){
    String res;
    for (int i = 0; i < (int)keys.size(); ++i){
        if (keys[i] == U"Right"){
            res += U"->";
        } else if (keys[i] == U"Left"){
            res += U"<-";
        } else if (keys[i] == U"0x5b"){
            res += U"Windows";
        } else{
            res += keys[i];
        }
        if (i < (int)keys.size() - 1){
            res += U"+";
        }
    }
    return res;
}

std::vector<String> get_all_inputs(bool *down_found){
    const Array<Input> raw_keys = Keyboard::GetAllInputs();
    *down_found = false;
    std::unordered_set<String> keys;
    for (const auto& key : raw_keys){
        *down_found |= key.down();
        keys.emplace(key.name());
    }
    std::vector<String> res;
    if (keys.find(U"Ctrl") != keys.end()){
        res.emplace_back(U"Ctrl");
    }
    if (keys.find(U"Shift") != keys.end()){
        res.emplace_back(U"Shift");
    }
    if (keys.find(U"Alt") != keys.end()){
        res.emplace_back(U"Alt");
    }
    for (String key: keys){
        if (key == U"Ctrl"){
            continue;
        }
        if (key == U"Shift"){
            continue;
        }
        if (key == U"Alt"){
            continue;
        }
        res.emplace_back(key);
    }
    return res;
}

class Shortcut_keys{
public:
    std::vector<Shortcut_key_elem> shortcut_keys;
public:
    void init(String file){
        shortcut_keys = shortcut_keys_default;
        JSON json = JSON::Load(file);
        std::unordered_set<String> name_list;
        for (Shortcut_key_elem &elem: shortcut_keys_default){
            name_list.emplace(elem.name);
        }
        for (const auto& object: json){
            if (name_list.find(object.key) == name_list.end()){
                std::cerr << "ERR shortcut key name not found " << object.key.narrow() << std::endl;
                continue;
            }
            for (int i = 0; i < (int)shortcut_keys.size(); ++i){
                if (shortcut_keys[i].name == object.key){
                    shortcut_keys[i].keys.clear();
                    for (const auto &key_name: object.value.arrayView()){
                        shortcut_keys[i].keys.emplace_back(key_name.getString());
                    }
                }
            }
        }
    }

    void check_shortcut_key(String *shortcut_name_down, String *shortcut_name_pressed){
        bool down_found = false;
        std::vector<String> keys = get_all_inputs(&down_found);

        //std::cerr << "keys size " << keys.size() << " down found " << down_found << std::endl;
        //for (const String& key : keys){
        //    std::cerr << key.narrow() << " ";
        //}
        //std::cerr << std::endl;

        *shortcut_name_down = SHORTCUT_KEY_UNDEFINED;
        *shortcut_name_pressed = SHORTCUT_KEY_UNDEFINED;
        for (const Shortcut_key_elem &elem: shortcut_keys){
            if (keys.size() && keys.size() == elem.keys.size()){
                bool matched = true;
                for (const String& key : keys){
                    //std::cerr << key.narrow() << " " << (std::find(elem.keys.begin(), elem.keys.end(), key) == elem.keys.end()) << std::endl;
                    if (std::find(elem.keys.begin(), elem.keys.end(), key) == elem.keys.end()){
                        matched = false;
                    }
                }
                if (matched){
                    if (down_found){
                        *shortcut_name_down = elem.name;
                    }
                    *shortcut_name_pressed = elem.name;
                    return;
                }
            }
        }
    }

    String get_shortcut_key_str(String name){
        for (const Shortcut_key_elem &elem: shortcut_keys){
            if (elem.name == name){
                return generate_key_str(elem.keys);
            }
        }
        return SHORTCUT_KEY_UNDEFINED;
    }

    String get_shortcut_key_description(String name){
        for (const Shortcut_key_elem &elem: shortcut_keys){
            if (elem.name == name){
                String res;
                for (int i = 0; i < (int)elem.description_keys.size(); ++i){
                    res += language.get(elem.description_keys[i]);
                    if (i < (int)elem.description_keys.size() - 1){
                        res += U"> ";
                    }
                }
                return res;
            }
        }
        return SHORTCUT_KEY_UNDEFINED;
    }

    void change(int idx, std::vector<String> keys){
        shortcut_keys[idx].keys.clear();
        for (String key: keys){
            shortcut_keys[idx].keys.emplace_back(key);
        }
    }

    void del(int idx){
        shortcut_keys[idx].keys.clear();
    }
};

Shortcut_keys shortcut_keys;
