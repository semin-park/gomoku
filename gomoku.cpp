//
//  gomoku.cpp
//  gomoku
//
//  Created by Semin Park on 25/04/2019.
//  Copyright © 2019 Semin Park. All rights reserved.
//

#include "gomoku.hpp"

#include <iostream>
#include <iomanip>

Gomoku::Gomoku() { 
    // pass
}


/*
 * Core functions. Make sure these functions are as optimized as possible.
 */

State Gomoku::reset() const
{
    ID id {{-1,-1}};
    Board board = torch::zeros({c_in, size, size}, torch::kUInt8);

    Positions pos;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            pos.insert({i,j});
        }
    }
    return {id, board, pos};
}

std::tuple<State, Reward, bool> Gomoku::step(const State& state, const Action& action) const
{
    ID id(get_id(state));
    id.emplace_back(action);
    Board board(get_board(state).clone());

    // throw if the given action is not valid
    int player = get_player(state);
    if (player == 7) {
        print(state);
        std::cout << board << std::endl;
        throw std::runtime_error("Player == 7");
    }
    int next_player = (player + 1) % 2;

    auto actions = possible_actions(state, player);
    auto it = std::find(actions.begin(), actions.end(), action);
    if (it == actions.end()) {
        std::cout << board << std::endl;
        print(state);
        std::cout << "Possible actions: ";
        for (auto& a : actions) {
            std::cout << action_string(a) << ',';
        }
        std::cout << std::endl;
        std::cout << "Requested action: " << action_string(action) << std::endl;
        throw std::runtime_error("<Gomoku::step> Action is not valid!");
    }
    actions.erase(it);

    
    int i = action[0];
    int j = action[1];
    
    board[player][i][j] = 1;

    
    int status = check_win(board, player, action);
    bool finished = false;
    if (status != CONTINUE) {
        finished = true;
        next_player = 7;
    }
    board.slice(0,2,3) = next_player;
    
    Reward reward = torch::zeros({2});
    if (status == WIN) {
        float data[] = {1,-1};
        reward = torch::from_blob(data, {2}).clone();
        if (player != 0)
            reward *= -1;
    }

    State next_state{id, board, actions};
    return {next_state, reward, finished};
}

std::set<Action> Gomoku::possible_actions(const State& state, int player) const
{
    return get_positions(state);
}


/*
 * Simple getters.
 */

int Gomoku::get_num_players() const
{
    return num_players;
}

ID Gomoku::get_id(const State& state) const
{
    return std::get<0>(state);
}

Board Gomoku::get_board(const State& state) const
{
    return std::get<1>(state);
}

int Gomoku::get_player(const State& state) const
{
    return get_board(state)[2][0][0].item<int>();
}

auto Gomoku::get_board_shape() const
    -> decltype(Gomoku::board_shape)
{
    return board_shape;
}

auto Gomoku::get_action_shape() const
    -> decltype(action_shape)
{
    return action_shape;
}

int Gomoku::get_state_channels() const
{
    return c_in;
}

int Gomoku::get_action_channels() const
{
    return c_out;
}

int Gomoku::get_board_size() const
{
    return size;
}



/*
 *  Helper functions
 */

State Gomoku::fast_forward(State state, std::vector<Action> actions)
{
    Reward r; bool d;
    for (auto& action : actions) {
        std::tie(state, r, d) = step(state, action);
    }
    return state;
}

std::string Gomoku::action_string(const Action& action)
{
    int i = action[0];
    int j = action[1];
    return "(" + std::to_string(i) + "," + std::to_string(j) + ")";
}

State Gomoku::copy(const State& other) const
{
    ID id(get_id(other));
    Board board(get_board(other).clone());
    Positions pos(get_positions(other));
    return {id, board, pos};
}

void Gomoku::print(const State& state) const
{
    static std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    const auto& board_ = get_board(state);
    auto board = board_.slice(0,0,1) + 2 * board_.slice(0,1,2);
    auto board_a = board.accessor<uint8_t, 3>();
    
    std::cout << "   ";
    for (char c : alphabet.substr(0,size)) std::cout << c << ' ';
    std::cout << std::endl;
    
    char mark;
    for (int i = 0; i < size; i++) {
        
        std::cout << std::setw(2) << i << ' ';
        for (int j = 0; j < size; j++) {
            
            mark = '.';
            if (board_a[0][i][j] == 1) {
                mark = 'O';
            } else if (board_a[0][i][j] == 2) {
                mark = 'X';
            } else if (board_a[0][i][j] > 2) {
                mark = '?';
            }
            std::cout << mark << ' ';
            
        }
        std::cout << std::endl;
    }
}



/*
 *  Private functions. These can be different across
 *  environments. Only used to help the public APIs.
 */
Positions Gomoku::get_positions(const State& state) const
{
    return std::get<2>(state);
}

int Gomoku::check_win(const Board& board, int player, const Action& action) const
{
    // No LOSE status, because the current player can't lose.
    if (check_row(board, player, action) ||
        check_column(board, player, action) ||
        check_main_diagonal(board, player, action) ||
        check_anti_diagonal(board, player, action))
        return WIN;

    auto matrix = board.slice(0,0,1) + board.slice(0,1,2);
    if (matrix.all().item<int>())
        return DRAW;

    return CONTINUE;
}

bool Gomoku::check_main_diagonal(const Board& board, int player, const Action& action) const
{
    int i = action[0];
    int j = action[1];

    auto board_a = board.accessor<uint8_t, 3>();

    int p, q;
    for (p = i, q = j; p >= 0 && q >= 0; p--, q--) {
        if (board_a[player][p][q] == 0)
            break;
    }

    int r, s;
    for (r = i, s = j; r < size && s < size; r++, s++) {
        if (board_a[player][r][s] == 0)
            break;
    }
    return (r - p > criteria) ? true : false;
}

bool Gomoku::check_anti_diagonal(const Board& board, int player, const Action& action) const
{
    int i = action[0];
    int j = action[1];

    auto board_a = board.accessor<uint8_t, 3>();

    int p, q;
    for (p = i, q = j; p >= 0 && q < size; p--, q++) {
        if (board_a[player][p][q] == 0)
            break;
    }

    int r, s;
    for (r = i, s = j; r < size && s >= 0; r++, s--) {
        if (board_a[player][r][s] == 0)
            break;
    }
    return (r - p > criteria) ? true : false;
}

bool Gomoku::check_row(const Board& board, int player, const Action& action) const
{
    int i = action[0];
    int j = action[1];

    auto board_a = board.accessor<uint8_t, 3>();

    int l, r;
    for (l = j; l >= 0; l--) {
        if (board_a[player][i][l] == 0)
            break;
    }

    for (r = j; r < size; r++) {
        if (board_a[player][i][r] == 0)
            break;
    }
    return (r - l > criteria) ? true : false;
}

bool Gomoku::check_column(const Board& board, int player, const Action& action) const
{
    int i = action[0];
    int j = action[1];

    auto board_a = board.accessor<uint8_t, 3>();

    int u, d;
    for (u = i; u >= 0; u--) {
        if (board_a[player][u][j] == 0)
            break;
    }

    for (d = i; d < size; d++) {
        if (board_a[player][d][j] == 0)
            break;
    }
    return (d - u > criteria) ? true : false;
}