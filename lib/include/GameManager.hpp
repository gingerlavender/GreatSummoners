#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include <iostream>
#include <limits>
#include <cstring>
#include <memory>
#include "GameView.hpp" 

enum Choices {
    SUMMON = 1,
    ENEMIES_LIST,
    TEAMMATES_LIST, 
    INFO, 
    ACCUMULATE,
    UPGRADE,
    MOVE,
    DAMAGE,
    EXIT
};

enum Modes {
    NEW_GAME = 1,
    LOAD_SAVE
};

class Game;
class Summoner;

class GameManager {
    public:
        template<class T>
        T get_num(T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max()) {
            T num;
            do {
                std::cin >> num;
                if (std::cin.eof()) {
                    throw std::runtime_error("Have a nice day!");
                } else if (std::cin.bad()) {
                    throw std::runtime_error(std::string("Failed to read number: ") + strerror(errno));
                } else if (std::cin.fail() || num < min || num > max) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Try again!\n\n";
                } else {
                    return num;
                }
            } while (true);
            return num;
        }
        void process_actions(Game& game, Summoner& player);
        void start_menu(Game& game, const std::string& units_dir);
};

#endif
