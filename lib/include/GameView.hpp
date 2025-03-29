#ifndef GAME_RENDERER
#define GAME_RENDERER

#include <string>
#include "descriptors.hpp"

class Game;
class Summoner;

class GameView {
    public:
        std::string get_hp(double hp);
        std::string get_short_name(const std::string& name); 
        void draw_field(Game& game);
        void print_menu();
        void print_skills(Game& game, Summoner& player);
        void print_team(Game& game, Summoner& player, Team team);
        void print_parameters(Game& game, Summoner& player);
        void print_schools(Game& game);
};

#endif
