#include "../include/game.hpp"
#include <cmath>
#include <limits>
#include <random>
#include <thread>

void BaseUnit::death(Game& game) {
    current_HP() = 0;
}

void RealUnit::move(Game& game, int x_pos, int y_pos) {
    if (abs(x() - x_pos) > characteristics().speed || abs(y() - y_pos) > characteristics().speed) {
        throw std::invalid_argument("Your speed is not enough!");
    } else if (!game.is_avialable(x_pos, y_pos)) {
        throw std::invalid_argument("This cell is unavialable");
    } else {
        x() = x_pos;
        y() = y_pos;
    }
}

void RealUnit::update_amount() {
    characteristics().amount = std::ceil(current_HP() / characteristics().entity_HP);
}

void RealUnit::take_damage(double damage) {
    current_HP() = current_HP() - damage;
    update_amount();
}

double RealUnit::damage_coefficient(SchoolsTable& table, unit_t enemy) {
    double coefficient = 1.0;
    if (RealUnit* real_unit = dynamic_cast<RealUnit*>(enemy.get())) {
        const School& school = table.get_school(characteristics().school);
        const School& other_school = table.get_school(real_unit->characteristics().school);
        if (school > other_school) {
            coefficient = 1.2;
        } else if (other_school > school) {
            coefficient = 0.8;
        }
    }
    return coefficient;
}

void RealUnit::make_damage(Game& game, unit_t enemy) {
    enemy->take_damage(enemy->damage_coefficient(game.schools_table(), enemy) * damage());
    if (enemy->current_HP() <= 0) {
        enemy->death(game);
    }
}

void RealUnit::make_turn(Game& game, Team self_team) {
    if (current_HP() <= 0) {
        return;
    }
    unit_t closest_enemy = game.find_closest_enemy(x(), y(), self_team);
    try {
        if (abs(x() - closest_enemy->x()) > characteristics().speed * 2) {
            if (x() - closest_enemy->x() < 0) {
                move(game, x() + characteristics().speed > FIELD_WEIGHT ? FIELD_WEIGHT - 1 : x() + characteristics().speed, y());
            } else {
                move(game, x() - characteristics().speed < 0 ? 0 : x() - characteristics().speed, y());
            }
            return;
        } else if (abs(y() - closest_enemy->y()) > characteristics().speed * 2) {
            if (y() - closest_enemy->y() < 0) {
                move(game, x(), y() + characteristics().speed > FIELD_HEIGHT ? FIELD_HEIGHT - 1 : y() + characteristics().speed);
            } else {
                move(game, x(), y() - characteristics().speed < 0 ? 0 : y() - characteristics().speed);
            }
            return;
        } 
        make_damage(game, closest_enemy);
    } 
    catch (std::invalid_argument& e) {
        for (int i = 0; i <= characteristics().speed; ++i) {
            for (int j = 0; j <= characteristics().speed; ++j) {
                if (i == 0 && j == 0) { continue; }
                if (game.is_avialable(x() + i, y() + j)) {
                    move(game, x() + i, y() + j);
                    return;
                } else if (game.is_avialable(x() - i, y() - j)) {
                    move(game, x() - i, y() - j);
                    return;
                }
            }
        }
        return;
    }
}

void MoralUnit::make_turn(Game& game, Team self_team) {
    if (current_HP() <= 0) {
        return;
    }
    RealUnit::make_turn(game, self_team);
    balance_morality();
}

void MoralUnit::increase_morality(double morality) {
    if (characteristics().morality.value() + morality >= MAX_MORALITY) {
        characteristics().morality = MAX_MORALITY;
    } else {
        characteristics().morality = characteristics().morality.value() + morality;
    }
}

void MoralUnit::decrease_morality(double morality) {
    if (characteristics().morality.value() - morality <= MIN_MORALITY) {
        characteristics().morality = MIN_MORALITY;
    } else {
        characteristics().morality = characteristics().morality.value() - morality;
    }
}

void MoralUnit::balance_morality() {
    if (characteristics().morality.value() < 0) {
        if (characteristics().morality.value() + 0.05 > 0) {
            characteristics().morality = 0;
        } else {
            increase_morality(0.05);
        }
    } else if (characteristics().morality.value() > 0) {
        if (characteristics().morality.value() - 0.05 < 0) {
            characteristics().morality = 0;
        } else {
            decrease_morality(0.05);
        }
    }
}

void MoralUnit::make_damage(Game& game, unit_t enemy) {
    enemy->take_damage(damage_coefficient(game.schools_table(), enemy) * (1.0 + characteristics().morality.value()) * damage());
    if (enemy->current_HP() <= 0) {
        enemy->death(game);
        increase_morality(0.25);
    }
}

void MoralUnit::take_damage(double damage) {
    current_HP() = current_HP() - damage;
    int temp_amount = characteristics().amount;
    update_amount();
    decrease_morality((temp_amount - characteristics().amount) * 0.01);
}

void RessurectionUnit::make_turn(Game& game, Team self_team) {
    if (current_HP() <= 0) {
        return;
    }
    if (characteristics().amount < characteristics().max_amount) {
        try_to_ressurect();
    }
    unit_->make_turn(game, self_team);
}

void RessurectionUnit::try_to_ressurect() {
    std::random_device rd{};
    std::mt19937 gen{rd()};
    try_to_ressurect(gen);
}

void RessurectionUnit::try_to_ressurect(std::mt19937 gen) {
    std::geometric_distribution<> d;
    int dead = characteristics().max_amount - characteristics().amount;
    int to_be_ressurected = 0;
    if (dead != 0) {
        do {
            to_be_ressurected = std::round(d(gen));
        } while (to_be_ressurected > (characteristics().max_amount - characteristics().amount));
    }
    characteristics().amount += to_be_ressurected;
    current_HP() += characteristics().entity_HP * to_be_ressurected;
}

void Summoner::make_turn(Game& game, Team self_team) {
    if (current_HP() <= 0) {
        return;
    }
    characteristics().left_XP += game.get_xp();
    if (self_team == PLAYER) {
        game.players_turn(*this);
        return;
    }
    std::tuple<std::string, std::string, double> preferable = {"NULL", "NULL", 0};
    for (auto school : characteristics().schools_knowledge) {
        for (auto skill : game.schools_table().get_school(school.first).skills) {
            if (skill.characteristics.damage >= get<double>(preferable) && characteristics().current_energy >= skill.required_energy && school.second >= skill.min_knowledge) {
                get<0>(preferable) = skill.characteristics.school;
                get<1>(preferable) = skill.name;
                get<double>(preferable) = skill.characteristics.damage;
            }
        }
    }
    if (get<0>(preferable) == "NULL") {
        accumulate_energy();
        return;
    }
    for (int i = 0; i <= 1; ++i) {
        for (int j = 0; j <= 1; ++j) {
            if (i == 0 && j == 0) { continue; }
            if (game.is_avialable(x() + i, y() + j)) {
                summon_unit(game, get<0>(preferable), get<1>(preferable), x() + i, y() + j);
                return;
            } else if (game.is_avialable(x() - i, y() - j)) {
                summon_unit(game, get<0>(preferable), get<1>(preferable), x() - i, y() - j);
                return;
            }
        }
    }
}

void Kamikaze::damage_all_enemies(Game& game, Team self_team) {
    size_t threads_amount = std::thread::hardware_concurrency() == 0 ? 12 : std::thread::hardware_concurrency();
    std::vector<std::thread> threads(threads_amount);
    size_t enemies_amount = self_team == PLAYER ? game.enemies().size() : game.teammates().size();
    const auto& enemies = self_team == PLAYER ? game.enemies() : game.teammates();
    for (size_t i = 0; i < threads_amount; ++i) {
        size_t start_i = i * enemies_amount / threads_amount;
        size_t end_i = std::min((i + 1) * enemies_amount / threads_amount, enemies_amount);
        auto start = std::next(enemies.begin(), start_i);
        auto end = std::next(enemies.begin(), end_i);
        threads[i] = std::thread([&, start, end](){ for (auto j = start; j < end; ++j) { (*j)->take_damage(damage()); } });
    }
    for (auto& thread : threads) {
        thread.join();
    }
}

void Kamikaze::make_turn(Game& game, Team self_team) {
    if (current_HP() <= 0) {
        return;
    }
    damage_all_enemies(game, self_team);
    current_HP() = 0;
}

void Summoner::accumulate_energy() {
    if (characteristics().current_energy == 0) {
        characteristics().current_energy += 10.0;
    } else if (characteristics().current_energy * characteristics().accumulation_coefficient >= characteristics().max_energy) {
        characteristics().current_energy = characteristics().max_energy;
    } else {
        characteristics().current_energy *= characteristics().accumulation_coefficient;
    }
}

void Summoner::summon_unit(Game& game, const std::string& school_name, const std::string& skill_name, size_t x, size_t y) {
    Skill& skill = game.schools_table().get_skill(school_name, skill_name);
    if (characteristics().schools_knowledge[skill.characteristics.school] < skill.min_knowledge) {
        throw std::runtime_error("Knowledge of this school is not enough to use this skill!");
    } else if (characteristics().current_energy < skill.required_energy) {
        throw std::runtime_error("Your energy level is not enough to use this skill!");
    }
    game.deploy_unit(x, y, skill.create(skill.characteristics), characteristics().team);
    characteristics().current_energy -= skill.required_energy;
}

void Summoner::upgrade_school(std::string& school_name) {
    if (characteristics().left_XP < 50.0) {
        throw std::runtime_error("You haven't enough XP to upgrade!");
    }
    characteristics().schools_knowledge[school_name] += 10.0;
    characteristics().left_XP -= 50.0;
}

double Summoner::damage_coefficient(SchoolsTable& table, unit_t enemy) {
    return 1.0;
}

void Summoner::move(Game& game, int x_pos, int y_pos) {
    if (abs(x() - x_pos) > 1 || abs(y() - y_pos) > 1) {
        throw std::invalid_argument("You can't go to that cell!");
    } else if (!game.is_avialable(x_pos, y_pos)) {
        throw std::invalid_argument("This cell is unavialable");
    } else {
        x() = x_pos;
        y() = y_pos;
    }
}

void Summoner::take_damage(double damage) {
    current_HP() = current_HP() - damage;
}

void Summoner::make_damage(Game& game, unit_t enemy) {
    enemy->take_damage(damage());
    if (enemy->current_HP() <= 0) {
        enemy->death(game);
    }
}
