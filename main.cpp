//
//  main.cpp
//  gomoku
//
//  Created by Semin Park on 25/04/2019.
//  Copyright © 2019 Semin Park. All rights reserved.
//

#include <iostream>
#include <string>

#include "gomoku.hpp"
#include "types.hpp"

Action get_action(int turn, int player)
{
    char mark = 'O';
    if (player == 1)
        mark = 'X';
    std::cout << std::endl << "Turn " << turn << " (Player " << mark << "): ";
    std::string str;
    getline(std::cin, str);

    if (str == "q" || str == "quit" || str == "exit") return {-1,-1};
    
    char a = str[0];
    char b = str[2];
    
    int i = int(a - '0');
    int j = int(b - 'A');
    // int j = int(b - '0');
    
    std::cout << "Action: " << i << ',' << j << std::endl;
    
    return {i,j};
}

int main(int argc, const char * argv[]) {
    
    Gomoku& env = Gomoku::get();
    
    State state;
    Reward reward;
    bool done = false;
    
    state = env.reset();
    int player = env.get_player(state);

    // state = env.fast_forward(state, {
    //     {4,4},
    //     {3,3},
    //     {4,2},
        
    // });
    
    std::cout << "Begin" << std::endl;
    env.print(state);
    
    int turn = 1;
    while (!done) {
        Action action = get_action(turn, player);
        if (action[0] == -1) break;
        try {
            std::tie(state, reward, done) = env.step(state, action);
        } catch (std::exception& e) {
            std::cout << "Wrong action. Try again." << std::endl;
            std::cout << "<Main> Error: " << e.what() << std::endl;
            continue;
        }
        
        player = env.get_player(state);
        turn++;
        
        auto reward_a = reward.accessor<float, 1>();

        if (done)
            std::cout << "Game finished. Reward: " << reward_a[0] << ',' << reward_a[1] << std::endl;
        env.print(state);
        
    }
    
    return 0;
}
