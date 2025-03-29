#include "../include/game.hpp"
#include <limits>
#include <iostream>
#include <chrono>

void GameManager::start_menu(Game& game, const std::string& units_dir) {
    int choice;
    std::cout << "1. Start new game\n";
    std::cout << "2. Load save\n\n";
    choice = get_num<int>();
    std::cout << "\n";
    if (choice == LOAD_SAVE) {
        std::string path;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\n" << "Enter path to save:\n";
        std::getline(std::cin, path);
        std::cout << "\n";
        game.read_save(path, units_dir);
    }
}

void GameManager::process_actions(Game& game, Summoner& player) {
    bool turn_made = false;
    do {
        game.view().draw_field(game);
        game.view().print_menu();
        int choice = get_num<int>();
        std::cout << "\n\n";
        switch (choice) {
            case SUMMON: 
                {
                    game.view().print_skills(game, player);
                    std::string school;
                    std::string name;
                    std::cout << "Enter school name:\n";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::getline(std::cin, school);
                    std::cout << "\n" << "Enter skill name:\n";
                    std::getline(std::cin, name);
                    std::cout << "\n" << "Enter x and y where to summon:\n";
                    int x = get_num<int>();
                    int y = get_num<int>();
                    std::cout << "\n\n";
                    try {
                        player.summon_unit(game, school, name, x, y);
                        turn_made = true;
                    }
                    catch (std::exception& e) {
                        std::cout << e.what() << "\n\n";
                    }
                }
                break;
            case ENEMIES_LIST:
                game.view().print_team(game, player, ENEMY);
                break;
            case TEAMMATES_LIST:
                game.view().print_team(game, player, PLAYER);
                break;
            case INFO:
                game.view().print_parameters(game, player);
                break;
            case ACCUMULATE:
                player.accumulate_energy();
                turn_made = true;
                break;
            case UPGRADE:
                {
                    game.view().print_schools(game);
                    std::string school;
                    std::string name;
                    std::cout << "Enter school name:\n";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::getline(std::cin, school);
                    try {
                        player.upgrade_school(school);
                        turn_made = true;
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << e.what() << "\n\n";
                    }
                    break;
                }
            case MOVE:
                    {
                        int x, y;
                        std::cout << "Enter x coordinate:\n\n";
                        x = get_num<int>();
                        std::cout << "\nEnter y coordinate:\n\n";
                        y = get_num<int>();
                        std::cout << "\n";
                        try {
                            player.move(game, x, y);
                            turn_made = true;
                        }
                        catch (const std::exception& e)
                        {
                            std::cout << e.what() << "\n\n";
                        }
                        break;
                    }
            case DAMAGE:
                    {
                        int x, y;
                        std::cout << "Enter x coordinate:\n\n";
                        x = get_num<int>();
                        std::cout << "\nEnter y coordinate:\n\n";
                        y = get_num<int>();
                        std::cout << "\n";
                        if (!game.accessible_for_player(player, x, y)) {
                            std::cout << "Too large distance\n\n";
                        }
                        try {
                            auto enemy = game.find_enemy(x, y, PLAYER);
                            player.make_damage(game, enemy);
                            turn_made = true;
                        }
                        catch (const std::exception& e)
                        {
                            std::cout << e.what() << "\n\n";
                        }
                        break;
                    }
            case EXIT:
            {
                std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                char buf[128] = {0};
                std::strftime(buf, sizeof(buf), "%Y-%m-%d-%H-%M-%S", std::localtime(&now));
                game.write_save("../../data/UserSaves/" + std::string(buf) + ".json");
                std::cout << "Bye!\n\n";
                game.is_active() = false;
                turn_made = true;
                break;
            }
            default:
                std::cout << "Oops... No such option!\n\n";
                break;
        }
    } while (!turn_made);
}
