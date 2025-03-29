#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <cstring>
#include "../lib/include/game.hpp"
#include "../lib/include/factory.hpp"

TEST_CASE("Matrix") {
    SECTION("Constructors") {
        Matrix<int,2,2> matrix = {1, 0, 0, 1};
        Matrix<int,0,0> empty;
        Matrix<int, 2, 2> other_matrix;
        Matrix<int, 2, 2> one_more_matrix;
        REQUIRE((matrix.at(0, 0) == 1 && matrix.at(0, 1) == 0 && matrix.at(0, 1) == 0 && matrix.at(1, 1) == 1));
        REQUIRE_THROWS(matrix.at(2, 2));
        REQUIRE(matrix.size() == 4);
        REQUIRE(!matrix.empty());
        REQUIRE(empty.empty());
        other_matrix = matrix;
        REQUIRE(matrix == other_matrix);
        one_more_matrix = other_matrix;
        REQUIRE(matrix == one_more_matrix);
        Matrix copy(matrix);
        REQUIRE(matrix == copy);
        Matrix robber(copy);
        REQUIRE(robber == matrix);
    }
}

TEST_CASE("Game") {
    SummonerDescriptor p_sd{PLAYER, "MEPhI Student", 1.0, 1.0, 10.0, 1.2, 10.0, {}};
    SummonerDescriptor e_sd{ENEMY, "D.S. Telyakovskii", 1.0, 1.0, 999.0, 1.2, 10.0, {{"MSU", 100.0}}}; 
    SummonerDescriptor k_sd{PLAYER, "V.L. Kamynin", 1.0, 1.0, 999.0, 1.2, 10.0, {{"MSU", 100.0}}};
    UnitDescriptor ud{"Calculus", "MSU", 0.5, 4, 2.0, 2.0, 3, 0.5, 2.0, std::nullopt};
    UnitDescriptor ud1{"Commision", "MEPhI", 0.7, 4, 1.0, 2.0, 3, 0.5, 2.0, 0.0};
    Matrix<GameCell, FIELD_WEIGHT, FIELD_HEIGHT> field;
    Skill skill_calculus{ud, Factory::create_amoral_unit, "Classes", 0.0, 0.0, 0.0};
    Skill skill_commision{ud1, Factory::create_moral_unit, "Commision", 0.0, 0.0, 0.0};
    std::vector<Skill> v1 = {skill_calculus};
    std::vector<Skill> v2 = {skill_commision};
    std::vector<std::string> doms1 = {};
    std::vector<std::string> doms2 = {"MSU"};
    School msu{"MSU", v1, doms1}; 
    School mephi{"MEPhI", v2, doms2};
    std::unordered_map<std::string, School> table = {{"MSU", msu}, {"MEPhI", mephi}};
    SECTION("SchoolsTable") {
        SchoolsTable st;
        std::vector<std::string> vec;
        School mix{"Mix"};
        skill_calculus.characteristics.school = "Mix";
        skill_commision.characteristics.school = "Mix";
        st.add_school(mix);
        REQUIRE(st.get_school(mix.name).skills.size() == 0);
        st.add_skill(skill_calculus);
        st.add_skill(skill_commision);
        REQUIRE(st.get_skill(skill_calculus.characteristics.school, skill_calculus.name).characteristics.initiative == 0.5);
        REQUIRE(st.schools_amount() == 1);
        REQUIRE(st.skills_amount() == 2);
        REQUIRE_THROWS(st.get_skill("MEPhI", "Love"));
        auto summoner = std::make_shared<Summoner>(0, 0, p_sd);
        summoner->characteristics().left_XP = 50.0;
        summoner->upgrade_school(mix.name);
        REQUIRE(summoner->characteristics().schools_knowledge[mix.name] == 5.0);
        REQUIRE(summoner->characteristics().left_XP == 0.0);
        REQUIRE_THROWS(summoner->upgrade_school(mix.name));
    }
    SECTION("Game initialization and simple damage") {
        SchoolsTable st{table};
        Game game{st, field};
        game.deploy_unit(0, 0, std::make_shared<Summoner>(0, 0, p_sd), PLAYER);
        game.deploy_unit(1, 1, std::make_shared<Summoner>(0, 0, e_sd), ENEMY);
        game.deploy_unit(2, 2, Factory::create_amoral_unit(ud), PLAYER);
        game.deploy_unit(3, 3, Factory::create_amoral_unit(ud), ENEMY);
        REQUIRE(game.teammates()[0]->current_HP() == p_sd.current_HP);
        REQUIRE(game.teammates()[1]->current_HP() == ud.current_HP);
        REQUIRE(game.enemies()[0]->current_HP() == e_sd.current_HP);
        REQUIRE(game.enemies()[1]->current_HP() == ud.current_HP);
        game.teammates()[1]->make_damage(game, game.enemies()[1]);
        REQUIRE(game.enemies()[1]->current_HP() == ud.max_amount * ud.entity_HP - ud.damage);
        game.deploy_unit(4, 4, Factory::create_amoral_unit(ud), PLAYER);
        REQUIRE(game.teammates().size() == 3);
        game.teammates()[2]->take_damage(1000);
        game.remove_dead();
        REQUIRE(game.teammates().size() == 2);
        auto unit = Factory::create_moral_unit(ud1);
        unit->characteristics().morality = MAX_MORALITY;
        REQUIRE(unit->damage_coefficient(st, game.teammates()[1]) > 1.0);
        double HP_before = game.teammates()[1]->current_HP();
        unit->make_damage(game, game.teammates()[1]);
        double HP_after = game.teammates()[1]->current_HP();
        REQUIRE((HP_before - HP_after) - unit->damage() * 1.2 * 2.0 < 0.001);
        unit->take_damage(4.2);
        REQUIRE(unit->characteristics().amount == 2);
        REQUIRE(unit->characteristics().morality < MAX_MORALITY);
        REQUIRE(game.teammates()[1]->damage_coefficient(st, unit) < 1.0);
    }
    SECTION("Keeping lists sorted") {
        SchoolsTable st;
        Game game{st, field};
        auto unit_a = Factory::create_amoral_unit(ud);
        auto unit_b = Factory::create_amoral_unit(ud);
        auto unit_c = Factory::create_amoral_unit(ud);
        auto unit_d = Factory::create_amoral_unit(ud);
        unit_a->characteristics().initiative = 4;
        unit_b->characteristics().initiative = 3;
        unit_c->characteristics().initiative = 2;
        unit_d->characteristics().initiative = 1;
        game.deploy_unit(0, 0, unit_d, PLAYER);
        game.deploy_unit(1, 1, unit_b, PLAYER);
        game.deploy_unit(2, 2, unit_a, PLAYER);
        game.deploy_unit(3, 3, unit_c, PLAYER);
        game.game_start();
        REQUIRE(game.teammates()[0] == unit_a);
        REQUIRE(game.teammates()[1] == unit_b);
        REQUIRE(game.teammates()[2] == unit_c);
        REQUIRE(game.teammates()[3] == unit_d);
        auto unit_e = Factory::create_amoral_unit(ud);
        unit_e->characteristics().initiative = 5;
        game.deploy_unit(4, 4, unit_e, PLAYER);
        REQUIRE(game.teammates()[0] == unit_e);
    }
    SECTION("Morality") {
        auto unit = Factory::create_moral_unit(ud1);
        unit->increase_morality(0.2);
        REQUIRE(unit->characteristics().morality == 0.2);
        unit->increase_morality(1.0);
        REQUIRE(unit->characteristics().morality == MAX_MORALITY);
        unit->decrease_morality(3.0);
        REQUIRE(unit->characteristics().morality == MIN_MORALITY);
        unit->characteristics().morality = 0.08;
        unit->balance_morality();
        REQUIRE(unit->characteristics().morality == 0.03);
        unit->balance_morality();
        REQUIRE(unit->characteristics().morality == 0.0);
        unit->characteristics().morality = -0.08;
        unit->balance_morality();
        REQUIRE(unit->characteristics().morality == -0.03);
       unit->balance_morality();
        REQUIRE(unit->characteristics().morality == 0.0);
    }
    SECTION("Summoner") {
        SchoolsTable st{table};
        Game game{st, field};
        auto summoner = std::make_shared<Summoner>(0, 0, e_sd);
        game.deploy_unit(0, 0, summoner, ENEMY);
        summoner->summon_unit(game, "MSU", "Classes", 2, 2);
        REQUIRE(game.enemies().size() == 2);
        REQUIRE(game.enemies()[1]->current_HP() == ud.entity_HP * ud.max_amount);
        game.deploy_unit(1, 1, Factory::create_amoral_unit(ud), PLAYER);
        summoner->make_damage(game, game.teammates()[0]);
        REQUIRE(game.teammates()[0]->current_HP() == ud.max_amount * ud.entity_HP - e_sd.damage);
        REQUIRE(summoner->damage_coefficient(game.schools_table(), game.teammates()[0]) == 1.0);
        summoner->accumulate_energy();
        REQUIRE(summoner->characteristics().current_energy == summoner->characteristics().max_energy);
        double energy_before = 1.0;
        summoner->characteristics().current_energy = energy_before;
        summoner->accumulate_energy();
        REQUIRE(summoner->characteristics().current_energy == energy_before * summoner->characteristics().accumulation_coefficient);
        summoner->characteristics().current_energy = 0.0;
        game.schools_table().get_skill("MSU", "Classes").required_energy = 1.0;
        REQUIRE_THROWS(summoner->summon_unit(game, "MSU", "Classes", 2, 2));
        summoner->characteristics().current_energy = 2.0;
        summoner->characteristics().schools_knowledge["MSU"] = 10.0;
        game.schools_table().get_skill("MSU", "Classes").min_knowledge = 100.0;
        REQUIRE_THROWS(summoner->summon_unit(game, "MSU", "Classes", 2, 2)); 
    }
    SECTION("XP") {
        SchoolsTable st{table};
        Game game{st, field};
        auto summoner = std::make_shared<Summoner>(0, 0, p_sd);
        game.deploy_unit(0, 0, Factory::create_amoral_unit(ud), ENEMY);
        game.enemies()[0]->take_damage(1000000);
        game.remove_dead();
        summoner->characteristics().left_XP = game.get_xp();
        REQUIRE(summoner->characteristics().left_XP == ud.xp_for_destroy);
        REQUIRE(game.get_xp() == 0);
    }
    SECTION("Move and cells avialability") {
        SchoolsTable st{table};
        Game game{st, field};
        game.field().at(0, 0).type() = OBSTACLE;
        auto summoner = std::make_shared<Summoner>(0, 0, e_sd);
        REQUIRE_THROWS(game.deploy_unit(0, 0, summoner, PLAYER));
        game.field().at(0, 0).type() = LAND;
        game.deploy_unit(0, 0, summoner, PLAYER);
        REQUIRE_THROWS(game.deploy_unit(0, 0, Factory::create_amoral_unit(ud), PLAYER));
        REQUIRE_THROWS(summoner->move(game, 10, 10));
        summoner->move(game, 1, 1);
        REQUIRE((summoner->x() == 1 && summoner->y() == 1));
        auto other_unit = Factory::create_moral_unit(ud1);
        REQUIRE_NOTHROW(game.deploy_unit(0, 0, other_unit, PLAYER));
        REQUIRE_THROWS(other_unit->move(game, 1, 1));
        other_unit->move(game, 2, 2);
        REQUIRE_THROWS(summoner->move(game, 2, 2));
        REQUIRE_THROWS(other_unit->move(game, 50, 50));
    }
    SECTION("Ressurection") {
        std::mt19937 gen{123456};
        auto ru = Factory::create_ressurection_unit(ud);
        ru->characteristics().amount = ud.max_amount - 3;
        ru->try_to_ressurect(gen);
        REQUIRE(ru->characteristics().amount > ud.max_amount - 3);
    }
    SECTION("Death") {
        SchoolsTable st;
        Game game{st, field};
        auto unit = Factory::create_moral_unit(ud1);
        unit->characteristics().damage = 1000;
        game.deploy_unit(1, 1, Factory::create_amoral_unit(ud), ENEMY);
        unit->make_damage(game, game.enemies()[0]);
        REQUIRE(game.enemies()[0]->current_HP() == 0);
        game.remove_dead();
        REQUIRE(game.enemies().size() == 0);
        REQUIRE_NOTHROW(game.deploy_unit(1, 1, Factory::create_amoral_unit(ud), ENEMY));
        game.remove_unit(game.enemies()[0]);
        game.deploy_unit(0, 0, std::make_shared<Summoner>(0, 0, p_sd), PLAYER);
        REQUIRE_THROWS(unit->make_damage(game, game.teammates()[0]));
        auto killer = Factory::create_amoral_unit(ud);
        killer->characteristics().damage = 1000;
        auto victim = Factory::create_moral_unit(ud1);
        REQUIRE_NOTHROW(killer->make_damage(game, victim));
        REQUIRE(victim->current_HP() == 0);
        killer->current_HP() = 0.1;
        auto summoner = std::make_shared<Summoner>(0, 0, p_sd);
        summoner->make_damage(game, killer);
        REQUIRE(killer->current_HP() == 0);
    }
    SECTION("Tick") {
        SchoolsTable st{table};
        Game game{st, field};
        auto Telyakovskii = std::make_shared<Summoner>(10, 10, e_sd);
        auto Kamynin = std::make_shared<Summoner>(30, 30, k_sd);
        game.deploy_unit(10, 10, Telyakovskii, ENEMY);
        game.deploy_unit(30, 30, Kamynin, PLAYER);
        game.do_tick();
        REQUIRE(game.enemies().size() == 2);
        game.deploy_unit(11, 11, Factory::create_amoral_unit(ud), PLAYER);
        game.do_tick();
        REQUIRE((game.teammates()[1]->current_HP() <= ud.entity_HP * ud.max_amount || game.enemies()[1]->current_HP() <= ud.entity_HP * ud.max_amount));
        auto unit = Factory::create_moral_unit(ud1);
        game.deploy_unit(2, 2, unit, PLAYER);
        game.do_tick();
        REQUIRE((unit->x() != 2 || unit->y() != 2));
    }
    SECTION("Reading configuration files") {
        Game game{"../../data/Units/", "../../data/Skills/", "../../data/Schools/", "../../data/Summoners/Student.json", "../../data/Summoners/D.S.Telyakovskii.json", "../../data/Field/GameField.json"};
        REQUIRE(game.schools_table().schools_amount() == 2);
        REQUIRE(game.enemies()[0]->current_HP() == 20);
        REQUIRE(game.teammates()[0]->current_HP() == 30);
    }
    SECTION("Reading & writing save files") {
        Game game{"../../data/Units/", "../../data/Skills/", "../../data/Schools/", "../../data/Summoners/Student.json", "../../data/Summoners/D.S.Telyakovskii.json", "../../data/Field/GameField.json", "../../data/Saves/Save.json"};
        REQUIRE(game.teammates().size() == 2);
        REQUIRE(game.enemies().size() == 2);
        game.write_save("../../data/Saves/OneMoreSave.json");
        Game one_more_game{"../../data/Units/", "../../data/Skills/", "../../data/Schools/", "../../data/Summoners/Student.json", "../../data/Summoners/D.S.Telyakovskii.json", "../../data/Field/GameField.json", "../../data/Saves/OneMoreSave.json"};
        REQUIRE(game.teammates().size() == 2);
        REQUIRE(game.enemies().size() == 2);
    }
    SECTION("Kamikaze") {
        SchoolsTable st{table};
        Game game{st, field};
        for (int i = 0; i < 100; ++i) {
            game.deploy_unit(i, i, Factory::create_amoral_unit(ud), ENEMY);
        }
        auto kamikaze = Factory::create_kamikaze(ud);
        game.deploy_unit(100, 100, kamikaze, PLAYER);
        kamikaze->make_turn(game, PLAYER);
        REQUIRE(kamikaze->current_HP() == 0);
        for (const auto& enemy : game.enemies()) {
            REQUIRE(enemy->current_HP() == ud.max_amount * ud.entity_HP - ud.damage);
        }
    }
}
