#include "../include/game.hpp"
#include "../include/factory.hpp"
#include "../../../../json/single_include/nlohmann/json.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
using json = nlohmann::json;

bool Game::is_avialable(int x, int y) {
    if (x >= FIELD_WEIGHT || y >= FIELD_HEIGHT || x < 0 || y < 0) { return false; }
    return !(Game::field().at(x, y).type() == OBSTACLE || std::any_of(player_units_.begin(), player_units_.end(), [&](auto& unit) { return unit->x() == x && unit->y() == y; }) || std::any_of(enemy_units_.begin(), enemy_units_.end(), [&](auto& unit) { return unit->x() == x && unit->y() == y; }));
}

void Game::deploy_unit(int x, int y, std::shared_ptr<BaseUnit> unit, Team team) {
    if (!is_avialable(x, y)) {
        throw std::invalid_argument("This cell is unavialable");
    }
    unit->x() = x;
    unit->y() = y;
    if (team == PLAYER) {
        player_units_.insert(std::upper_bound(player_units_.begin(), player_units_.end(), unit, [](auto unit_a, auto unit_b){ return unit_a->initiative() > unit_b->initiative(); }), unit);
    } else {
        enemy_units_.insert(std::upper_bound(enemy_units_.begin(), enemy_units_.end(), unit, [](auto unit_a, auto unit_b){ return unit_a->initiative() > unit_b->initiative(); }), unit);
    }
}

void Game::remove_dead() {
    for (auto unit : player_units_) {
        if (unit->current_HP() <= 0 && typeid(*unit) == typeid(Summoner)) {
            game_over(ENEMY);
        }
    }
    std::erase_if(player_units_, [](auto unit){ return unit->current_HP() <= 0; });
    for (auto unit : enemy_units_) {
        if (unit->current_HP() <= 0) {
            add_xp(unit->xp_for_destroy());
            if (typeid(*unit) == typeid(Summoner)) {
                game_over(PLAYER);
            }
        }
    }
    std::erase_if(enemy_units_, [](auto unit){ return unit->current_HP() <= 0; });
}

void Game::remove_unit(std::shared_ptr<BaseUnit> unit) {
    if (std::erase(player_units_, unit) == 0) {
        std::erase(enemy_units_, unit);
    }
}

void Game::game_start() {
    std::sort(player_units_.begin(), player_units_.end(), [](auto unit_a, auto unit_b){ return unit_a->initiative() > unit_b->initiative(); });
    std::sort(enemy_units_.begin(), enemy_units_.end(), [](auto unit_a, auto unit_b){ return unit_a->initiative() > unit_b->initiative(); });
}

void Game::game_over(Team winner_team) {
    throw std::runtime_error(winner_team == PLAYER ? "Player has won" : "Player has lost");
    is_active() = false;
}

bool Game::accessible_for_player(Summoner& player, int x, int y) {
    return (abs(player.x() - x) >= 2 || abs(player.y() - y) >= 2) ? false : true;
}

std::shared_ptr<BaseUnit> Game::find_enemy(int x, int y, Team team) {
    auto& units = team == PLAYER ? enemies() : teammates();
    auto enemy = std::find_if(units.begin(), units.end(), [=](auto unit){ return unit->x() == x && unit->y() == y; });
    if (enemy == units.end()) {
        throw std::runtime_error("No such enemy!");
    } else {
        return *enemy;
    }
}

std::shared_ptr<BaseUnit> Game::find_closest_enemy(int x, int y, Team team) {
    if (team == ENEMY) {
        return *std::min_element(player_units_.begin(), player_units_.end(), [=](auto u_a, auto u_b){ return sqrt(pow(x - u_a->x(), 2) + pow(y - u_a->y(), 2)) < sqrt(pow(x - u_b->x(), 2) + pow(y - u_b->y(), 2)); });
    } else {
        return *std::min_element(enemy_units_.begin(), enemy_units_.end(), [=](auto u_a, auto u_b){ return sqrt(pow(x - u_a->x(), 2) + pow(y - u_a->y(), 2)) < sqrt(pow(x - u_b->x(), 2) + pow(y - u_b->y(), 2)); });
    }
}

void Game::do_tick() {
    auto teammates_copy = player_units_;
    auto enemies_copy = enemy_units_;
    auto p_iter = teammates_copy.begin();
    auto e_iter = enemies_copy.begin();
    while (p_iter != teammates_copy.end() && e_iter != enemies_copy.end()) {
        if ((*p_iter)->initiative() >= (*e_iter)->initiative()) {
            (*p_iter)->make_turn(*this, PLAYER);
            ++p_iter;
        } else if ((*p_iter)->initiative() < (*e_iter)->initiative()) {
            (*e_iter)->make_turn(*this, ENEMY);
            ++e_iter;
        }
    }
    if (p_iter == teammates_copy.end()) {
        std::for_each(e_iter, enemies_copy.end(), [&](auto unit){ unit->make_turn(*this, ENEMY); });
    } else {
        std::for_each(p_iter, teammates_copy.end(), [&](auto unit){ unit->make_turn(*this, PLAYER); });
    }
    remove_dead();
}

SchoolsTable Game::read_schools_table_(const std::string& units_dir, const std::string& skills_dir, const std::string& schools_dir) {
    std::unordered_map<std::string, UnitDescriptor> unit_map;
    std::unordered_map<std::string, Skill> skill_map;
    std::unordered_map<std::string, School> school_map;
    for (const auto& unit_json : std::filesystem::directory_iterator(units_dir)) {
        std::ifstream json_file(unit_json.path());
        json unit = json::parse(json_file);
        std::optional<double> morality = std::nullopt;
        if (unit["morality"] != "NULL") {
            morality = unit["morality"];
        } else {
            morality = std::nullopt;
        }
        UnitDescriptor ud(unit["name"], unit["school"], unit["initiative"], unit["max_amount"], unit["damage"], unit["entity_hp"], unit["speed"], unit["defence"], unit["xp_for_destroy"], morality);
        unit_map[ud.name] = ud;
    }
    for (const auto& skill_json : std::filesystem::directory_iterator(skills_dir)) {
        std::ifstream skill_file(skill_json.path());
        json skill = json::parse(skill_file);
        std::function<std::shared_ptr<BaseUnit>(UnitDescriptor&)> create;
        if (skill["type"] == "Moral") {
            create = Factory::create_moral_unit;
        } else if (skill["type"] == "Amoral") {
            create = Factory::create_amoral_unit;
        } else if (skill["type"] == "Ressurection") {
            create = Factory::create_ressurection_unit;
        } else if (skill["type"] == "Kamikaze") {
            create = Factory::create_kamikaze;
        }
        Skill sk(unit_map[skill["entity"]], create, skill["name"], skill["min_knowledge"], skill["required_energy"], skill["knowledge_coefficient"]);
        skill_map[sk.name] = sk;
    }
    for (const auto& school_json : std::filesystem::directory_iterator(schools_dir)) {
        std::ifstream school_file(school_json.path());
        json school = json::parse(school_file);
        std::vector<Skill> vec_skills;
        std::vector<std::string> vec_relations;
        for (auto& skill : school["skills"]) {
            vec_skills.push_back(skill_map[skill]);
        }
        for (auto& dominant_for : school["dominant_for"]) {
            vec_relations.push_back(dominant_for);
        }
        School sch(school["name"], vec_skills, vec_relations);
        school_map[sch.name] = sch;
    }
    return SchoolsTable(school_map);
}

std::shared_ptr<Summoner> Game::read_summoner_(const std::string& summoner_path, Team team) {
    std::ifstream summoner_file(summoner_path);
    json summoner = json::parse(summoner_file);
    std::unordered_map<std::string, double> schools_knowledge;
    for (auto& school : summoner["schools_knowledge"]) {
        schools_knowledge[school[0]] = school[1];
    }
    SummonerDescriptor descriptor(team, summoner["name"], summoner["initiative"], summoner["damage"], summoner["max_hp"], summoner["accumulation_coefficient"], summoner["max_energy"], schools_knowledge);
    return std::make_shared<Summoner>(team == PLAYER ? 10 : FIELD_WEIGHT - 10, FIELD_HEIGHT / 2, descriptor);
}

Matrix<GameCell, FIELD_WEIGHT, FIELD_HEIGHT> Game::read_field_(const std::string& field_path) {
    Matrix<GameCell, FIELD_WEIGHT, FIELD_HEIGHT> field;
    field.fill(LAND);
    std::ifstream field_file(field_path);
    json field_json = json::parse(field_file);
    for (auto& point : field_json["obstacles"]) {
        field.at(point[0], point[1]) = OBSTACLE;
    }
    return field;
}

void Game::write_save(const std::string& save_path) {
    std::ofstream save(save_path);
    save << "{\"units\":[";
    for (auto unit : player_units_) {
        save << "{\"type\":";
        if (typeid(*unit) == typeid(Summoner)) {
            save << "\"Summoner\",";
            save << "\"xp\":" << static_pointer_cast<Summoner>(unit)->characteristics().left_XP << ",";
        } else if (typeid(*unit) == typeid(RessurectionUnit)) {
            save << "\"Ressurection\",";
        } else if (typeid(*unit) == typeid(AmoralUnit)) {
            save << "\"Amoral\",";
        } else if (typeid(*unit) == typeid(MoralUnit)) {
            save << "\"Moral\",";
        } else if (typeid(*unit) == typeid(Kamikaze)) {
            save << "\"Kamikaze\","; 
        }
        save << "\"name\":" << "\"" << unit->name() << "\",";
        save << "\"hp\":" << unit->current_HP() << ",";
        save << "\"team\":" << "\"player\"" << ",";
        save << "\"x\":" << unit->x() << ",";
        save << "\"y\":" << unit->y() << "},";
    }
    for (auto unit : enemy_units_) {
        save << "{\"type\":";
        if (typeid(*unit) == typeid(Summoner)) {
            save << "\"Summoner\",";
            save << "\"xp\":" << static_pointer_cast<Summoner>(unit)->characteristics().left_XP << ",";
        } else if (typeid(*unit) == typeid(RessurectionUnit)) {
            save << "\"Ressurection\",";
        } else if (typeid(*unit) == typeid(AmoralUnit)) {
            save << "\"Amoral\",";
        } else if (typeid(*unit) == typeid(MoralUnit)){
            save << "\"Moral\",";
        } else if (typeid(*unit) == typeid(Kamikaze)) {
            save << "\"Kamikaze\",";
        }
        save << "\"name\":" << "\"" << unit->name() << "\",";
        save << "\"hp\":" << unit->current_HP() << ",";
        save << "\"team\":" << "\"enemy\"" << ",";
        save << "\"x\":" << unit->x() << ",";
        save << "\"y\":" << unit->y() << "},";
    }
    save.seekp(static_cast<long>(save.tellp()) - 1);
    save << "]}";
}

void Game::read_save(const std::string& save_path, const std::string& units_dir) {
    std::unordered_map<std::string, UnitDescriptor> unit_map;
    for (const auto& unit_json : std::filesystem::directory_iterator(units_dir)) {
        std::ifstream json_file(unit_json.path());
        json unit = json::parse(json_file);
        std::optional<double> morality = std::nullopt;
        if (unit["morality"] != "NULL") {
            morality = unit["morality"];
        } else {
            morality = std::nullopt;
        }
        UnitDescriptor ud(unit["name"], unit["school"], unit["initiative"], unit["max_amount"], unit["damage"], unit["entity_hp"], unit["speed"], unit["defence"], unit["xp_for_destroy"], morality);
        unit_map[ud.name] = ud;
    }
    std::ifstream save_file(save_path);
    json save = json::parse(save_file);
    for (auto& unit : save["units"]) {
        std::shared_ptr<BaseUnit> unit_ptr;
        if (unit["type"] == "Moral") {
            unit_ptr = Factory::create_moral_unit(unit_map[unit["name"]]);
        } else if (unit["type"] == "Amoral") {
            unit_ptr = Factory::create_amoral_unit(unit_map[unit["name"]]);
        } else if (unit["type"] == "Ressurection") {
            unit_ptr = Factory::create_ressurection_unit(unit_map[unit["name"]]);
        } else if (unit["type"] == "Summoner"){
            double xp = unit["xp"];
            unit["team"] == "player" ? teammates()[0]->current_HP() = unit["hp"] : enemies()[0]->current_HP() = unit["hp"];
            unit["team"] == "player" ? teammates()[0]->x() = unit["x"] : enemies()[0]->x() = unit["x"];
            unit["team"] == "player" ? teammates()[0]->y() = unit["y"] : enemies()[0]->y() = unit["y"];
            unit["team"] == "player" ? static_pointer_cast<Summoner>(teammates()[0])->characteristics().left_XP = xp : static_pointer_cast<Summoner>(enemies()[0])->characteristics().left_XP = xp;
            continue;
        }
        unit_ptr->current_HP() = unit["hp"];
        static_pointer_cast<RealUnit>(unit_ptr)->update_amount();
        deploy_unit(unit["x"], unit["y"], unit_ptr, unit["team"] == "player" ? PLAYER : ENEMY);
    }
}

Game::Game(const std::string& units_dir, const std::string& skills_dir, const std::string& schools_dir, const std::string& player_summoner_path, const std::string& enemy_summoner_path, const std::string& field_path) {
    field_ = read_field_(field_path);
    schools_table_ = read_schools_table_(units_dir, skills_dir, schools_dir);
    player_units_ = {read_summoner_(player_summoner_path, PLAYER)};
    enemy_units_ = {read_summoner_(enemy_summoner_path, ENEMY)};
}

Game::Game(const std::string& units_dir, const std::string& skills_dir, const std::string& schools_dir, const std::string& player_summoner_path, const std::string& enemy_summoner_path, const std::string& field_path, const std::string& save_path) {
    field_ = read_field_(field_path);
    schools_table_ = read_schools_table_(units_dir, skills_dir, schools_dir);
    player_units_ = {read_summoner_(player_summoner_path, PLAYER)};
    enemy_units_ = {read_summoner_(enemy_summoner_path, ENEMY)};
    read_save(save_path, units_dir);
}

void Game::players_turn(Summoner& player) {
    manager_.process_actions(*this, player);
}
