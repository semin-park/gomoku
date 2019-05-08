//
//  gomoku.hpp
//  gomoku
//
//  Created by Semin Park on 23/01/2019.
//  Copyright © 2019 Semin Park. All rights reserved.
//

#ifndef gomoku_hpp
#define gomoku_hpp

#include <array>
#include <set>
#include <sstream>
#include <string>
#include <tuple>

#include "types.hpp"

class Gomoku {
public:
#ifdef _LINUX
    using shape_type = long;
#else
    using shape_type = long long;
#endif // _LINUX
    
    using state_type = State;
    using reward_type = Reward;

    using id_type = ID;
    using board_type = Board;
    using action_type = Action;
    
private:
    enum { DRAW, WIN, LOSE, CONTINUE };

    int size {19}; // width and height
    int c_in {3};
    int c_out {1};
    int turn = 2; // index for turn
    int num_players = 2;

    std::array<shape_type, 3> board_shape {c_in, size, size}; // one for each player + one for indicating turn
    std::array<shape_type, 3> action_shape {c_out, size, size};

public:
    static Gomoku& get() {
        static Gomoku unique;
        return unique;
    }
    
    /*
     * Core functionalities. These are the meaty parts.
     */
    State reset() const;
    
    std::tuple<State, Reward, bool> step(const State& state, const Action& action) const;
    
    std::set<Action> possible_actions(const State& board, int player) const;


    /*
     * Simple getters.
     */
    int get_num_players() const;

    ID get_id(const State& state) const;
    
    Board get_board(const State& state) const;
    
    int get_player(const State& state) const;

    auto get_board_shape() const
        -> decltype(Gomoku::board_shape);
    
    auto get_action_shape() const
        -> decltype(Gomoku::action_shape);

    int get_state_channels() const;

    int get_action_channels() const;

    int get_board_size() const;



    /*
     * Helper functionalities.
     */
    State fast_forward(State state, std::vector<Action> actions);

    State copy(const State& other) const;
    
    std::stringstream to_string(const State& state, const Action& action = {}) const;

    static std::string action_string(const Action& action); // for debugging

private:
    // private methods
    Positions get_positions(const State& state) const;

    int check_win(const Board& board, int player, const Action& action) const;

    int check_main_diagonal(const Board& board, int player, const Action& action) const;

    int check_anti_diagonal(const Board& board, int player, const Action& action) const;

    int check_row(const Board& board, int player, const Action& action) const;

    int check_column(const Board& board, int player, const Action& action) const;
    
    // Constructor private
    Gomoku();

    // Five in a row
    int criteria = 5;
};
#endif /* gomoku_hpp */
