#ifndef GAME_HPP
#define GAME_HPP

#define FIELD_WEIGHT 40
#define FIELD_HEIGHT 40

#include "SchoolsTable.hpp"
#include "matrix.hpp"
#include "GameCell.hpp"
#include "GameView.hpp"
#include "GameManager.hpp"

class Game {
    private:
        bool is_active_ = true;
        GameManager manager_;
        GameView view_;
        using units_t = std::vector<std::shared_ptr<BaseUnit>>;
        units_t player_units_;
        units_t enemy_units_;
        Matrix<GameCell, FIELD_WEIGHT, FIELD_HEIGHT> field_;
        SchoolsTable schools_table_;
        double xp_to_collect_ = 0;
        SchoolsTable read_schools_table_(const std::string& units_dir, const std::string& skills_dir, const std::string& schools_dir);
        std::shared_ptr<Summoner> read_summoner_(const std::string& summoner_path, Team team);
        Matrix<GameCell, FIELD_WEIGHT, FIELD_HEIGHT> read_field_(const std::string& field_path);
    public:
        Game(const std::string& units_dir, const std::string& skills_dir, const std::string& schools_dir, const std::string& player_summoner_path, const std::string& enemy_summoner_path, const std::string& field_path);
        Game(const std::string& units_dir, const std::string& skills_dir, const std::string& schools_dir, const std::string& player_summoner_path, const std::string& enemy_summoner_path, const std::string& field_path, const std::string& save_path);
        Game(SchoolsTable& schools_table, Matrix<GameCell, FIELD_WEIGHT, FIELD_HEIGHT>& field) {
            schools_table_ = schools_table;
            field_ = field;
        }
        Game(units_t& p_units, units_t& e_units, SchoolsTable& schools_table, Matrix<GameCell, FIELD_WEIGHT, FIELD_HEIGHT>& field) {
            player_units_ = p_units;
            enemy_units_ = e_units;
            schools_table_ = schools_table;
            field_ = field;
        }
        bool& is_active() { return is_active_; }
        void write_save(const std::string& save_path);
        void read_save(const std::string& save_path, const std::string& units_dir);
        SchoolsTable& schools_table() { return schools_table_; }
        void add_xp(double xp) { xp_to_collect_ += xp; }
        double get_xp() { double tmp = xp_to_collect_; xp_to_collect_ = 0; return tmp; }
        void game_start();
        void game_over(Team winner_team);
        void deploy_unit(int x, int y, std::shared_ptr<BaseUnit> unit, Team team);
        void remove_unit(std::shared_ptr<BaseUnit> unit);
        void remove_dead();
        std::shared_ptr<BaseUnit> find_closest_enemy(int x, int y, Team team);
        bool is_avialable(int x, int y);
        Matrix<GameCell, FIELD_WEIGHT, FIELD_HEIGHT>& field() { return field_; }
        const Matrix<GameCell, FIELD_WEIGHT, FIELD_HEIGHT>& field() const { return field_; }
        const units_t& teammates() const { return player_units_; }
        const units_t& enemies() const { return enemy_units_; }
        units_t& teammates() { return player_units_; }
        units_t& enemies() { return enemy_units_; }
        GameView view() { return view_; }
        GameManager manager() { return manager_; }
        bool accessible_for_player(Summoner& player, int enemy_x, int enemy_y);
        void do_tick();
        void players_turn(Summoner& player);
        std::shared_ptr<BaseUnit> find_enemy(int x, int y, Team team);
};

#endif
