#include "../lib/include/game.hpp"
#include <iostream> 

int main() {
    Game game{"../../data/Units/", "../../data/Skills/", "../../data/Schools/", "../../data/Summoners/Student.json", "../../data/Summoners/D.S.Telyakovskii.json", "../../data/Field/GameField.json"};
    try {
        game.manager().start_menu(game, "../../data/Units/");
        while (game.is_active()) {
            game.do_tick();
        }
    }
    catch (const std::exception& e) 
    {
        std::cout << e.what() << "\n\n";
    }
}
