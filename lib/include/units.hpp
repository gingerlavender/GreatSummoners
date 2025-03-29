#ifndef UNITS_HPP
#define UNITS_HPP

/**
 * \file units.hpp
 * \brief Реализация существующих в игре отрядов.
 */

#include <memory>
#include <random>
#include "descriptors.hpp"

class SchoolsTable;
class Game;

/**
 * @brief Реализация базового юнита.
 */
class BaseUnit {
    private:
        int x_ = 0; ///< Координата по горизонтальной оси
        int y_ = 0; ///< Координата по вертикальной оси
    public:
        using unit_t = std::shared_ptr<BaseUnit>;
        /**
        * \brief Конструктор базового юнита.
        *
        * \param x Координата по горизонтальной оси
        * \param y Координата по вертикальной оси
        */
        BaseUnit(int x, int y) : x_(x), y_(y) {}
        /**
        * \brief Возвращает текущую позицию юнита по X.
        * 
        * \return Позиция юнита по X.
        */
        virtual int& x() {
            return x_;
        }
        /**
        * \brief Возвращает текущую позицию юнита по Y.
        * 
        * \return Позиция юнита по Y.
        */
        virtual int& y() {
            return y_;
        }
        /**
        * \brief Обработка смерти юнита.
        * 
        * \param game Текущий объект игры.
        */
        virtual void death(Game& game);
         /**
        * \brief Возвращает урон, наносимый юнитом.
        * 
        * \return Значение урона юнита.
        */
        virtual double damage() = 0;
        /**
        * \brief Получение коэффициента урона против конкретного врага.
        * 
        * \param table Таблица школ.
        * \param enemy Указатель на вражеский юнит.
        * \return Коэффициент урона.
        */
        virtual double damage_coefficient(SchoolsTable& table, unit_t enemy) = 0;
        /**
        * \brief Возвращает имя юнита.
        * 
        * \return Строка с именем юнита.
        */
        virtual const std::string& name() = 0;
        /**
        * \brief Возвращает текущее здоровье юнита.
        * 
        * \return Текущее здоровье юнита.
        */
        virtual double& current_HP() = 0;
        /**
        * \brief Возвращает текущее количество существ в отряде.
        * 
        * \return Текущее количество существ.
        */
        virtual int amount() = 0;
        /**
        * \brief Возвращает инициативу юнита.
        * 
        * \return Значение инициативы юнита.
        */
        virtual double& initiative() = 0;
        /**
        * \brief Возвращает количество опыта за уничтожения юнита.
        * 
        * \return Значение опыта.
        */
        virtual double xp_for_destroy() = 0;
         /**
        * \brief Получение урона.
        * 
        * \param damage Значение урона, которое необходимо применить.
        */
        virtual void take_damage(double damage) = 0;
        /**
        * \brief Нанесение урона другому юниту.
        * 
        * \param game Текущий объект игры.
        * \param enemy Указатель на вражеский юнит.
        */
        virtual void make_damage(Game& game, unit_t enemy) = 0;
        /**
        * \brief Перемещение юнита на новую позицию.
        * 
        * \param game Текущий объект игры.
        * \param x_pos Новая координата по X.
        * \param y_pos Новая координата по Y.
        * \throws std::invalid_argument Если новая позиция недоступна.
        */
        virtual void move(Game& game, int x, int y) = 0;
        /**
        * @brief Выполнение действий в текущем ходе.
        * 
        * @param game Текущий объект игры.
        * @param self_team Команда, к которой принадлежит юнит.
        */
        virtual void make_turn(Game&, Team self_team) = 0;
        virtual ~BaseUnit() = default;
};

/**
 * @brief Общий базовый класс для отрядов, не являющихся призывателями.
 */
class RealUnit : public BaseUnit {
    private:
        UnitDescriptor characteristics_; ///< Дескриптор с характеристиками отряда
    public:
        /**
        * \brief Конструктор общего базового класса не-призывателя.
        * 
        * \param x Координата по горизонтальной оси.
        * \param y Координата по вертикальной оси.
        * \param descriptor Дескриптор с характеристиками юнита.
        */
        RealUnit(int x, int y, UnitDescriptor& descriptor) : BaseUnit(x, y), characteristics_(descriptor) {}
        virtual UnitDescriptor& characteristics() {
        return characteristics_;
        }
        const std::string& name() override {
            return characteristics().name;
        }
        double damage() override {
            return characteristics().damage;    
        }
        double& initiative() override {
            return characteristics().initiative;
        }
        double& current_HP() override {
            return characteristics().current_HP;
        }
        int amount() override {
            return characteristics().amount;
        }
        void make_turn(Game&, Team self_team) override;
        void move(Game& game, int x, int y) override;
        void make_damage(Game& game, unit_t enemy) override;
        void take_damage(double damage) override;
        double xp_for_destroy() override { return characteristics().xp_for_destroy; }
        /**
        * \brief Обновляет количество активных юнитов на основе их текущего здоровья.
        */
        virtual void update_amount();
        double damage_coefficient(SchoolsTable& table, unit_t enemy) override;
};

/**
 * @brief Класс юнита с механикой морали.
 */
class MoralUnit : public RealUnit {
    public:
        /**
        * \brief Конструктор морального юнита.
        * 
        * \param x Координата по горизонтальной оси.
        * \param y Координата по вертикальной оси.
        * \param descriptor Дескриптор с характеристиками юнита.
        */
        MoralUnit(int x, int y, UnitDescriptor& descriptor) : RealUnit(x, y, descriptor) {}
         /**
        * \brief Наносит урон врагу с учетом морали.
        */
        void take_damage(double damage) override;
        /**
        * \brief Наносит урон врагу с учетом морали.
        */
        void make_damage(Game& game, unit_t enemy) override;
        /**
        * \brief Увеличивает мораль юнита.
        * 
        * \param morality Значение для увеличения морали.
        */
        virtual void increase_morality(double morality);
        /**
        * \brief Уменьшает мораль юнита.
        * 
        * \param morality Значение для уменьшения морали.
        */
        virtual void decrease_morality(double morality);
        /**
        * \brief Балансирует мораль юнита в зависимости от её текущего значения.
        */
        virtual void balance_morality();
        /**
        * \brief Выполняет действия в текущем ходе, включая балансировку морали.
        */
        virtual void make_turn(Game& game, Team self_team) override;
};

/**
 * @brief Класс аморального юнита, не имеющего механики морали.
 */
class AmoralUnit : public RealUnit {
    public:
        /**
        * \brief Конструктор аморального юнита.
        * 
        * \param x Координата по горизонтальной оси.
        * \param y Координата по вертикальной оси.
        * \param descriptor Дескриптор с характеристиками юнита.
        */
        AmoralUnit(int x, int y, UnitDescriptor& descriptor) : RealUnit(x, y, descriptor) {}
};

class RessurectionUnit : public RealUnit {
    private:
        std::shared_ptr<RealUnit> unit_;
    public:
        RessurectionUnit(int x, int y, UnitDescriptor& descriptor) : RealUnit(x, y, descriptor) {
            if (descriptor.morality == std::nullopt) {
                unit_ = std::make_shared<AmoralUnit>(x, y, descriptor);
            } else {
                unit_ = std::make_shared<MoralUnit>(x, y, descriptor);
            }
        }
        virtual std::shared_ptr<RealUnit>& unit() {
            return unit_;
        }
        virtual int& x() override {
            return unit_->x();
        }
        virtual int& y() override {
            return unit_->y();
        }
        UnitDescriptor& characteristics() override {
            return unit_->characteristics();
        } 
        void take_damage(double damage) override {
            unit_->take_damage(damage);
        };
        void make_damage(Game& game, unit_t enemy) override {
            unit_->make_damage(game, enemy);
        };
        void move(Game& game, int x, int y) override {
            unit_->move(game, x, y);   
        }
        void make_turn(Game&, Team self_team) override;
        double damage() override {
            return unit_->characteristics().damage;    
        }
        double& initiative() override {
            return unit_->characteristics().initiative;
        }
        double& current_HP() override {
            return unit_->characteristics().current_HP;
        }
        void death(Game& game) override {
            unit_->death(game);
        }
        void update_amount() override {
            unit_->update_amount();
        }
        double damage_coefficient(SchoolsTable& table, unit_t enemy) override {
            return unit_->damage_coefficient(table, enemy);
        }
        double xp_for_destroy() override { 
            return unit_->xp_for_destroy();
        }
        virtual void try_to_ressurect(); 
        virtual void try_to_ressurect(std::mt19937 gen);
};

class Kamikaze : public AmoralUnit {
    public:
        Kamikaze(int x, int y, UnitDescriptor& descriptor) : AmoralUnit(x, y, descriptor) {}
        void damage_all_enemies(Game& game, Team self_team);
        void make_turn(Game& game, Team self_team) override;
};

class Summoner : public BaseUnit {
    private:
        SummonerDescriptor characteristics_;
    public:
        Summoner(int x, int y, SummonerDescriptor& descriptor) : BaseUnit(x, y), characteristics_(descriptor) {}
        SummonerDescriptor& characteristics() {
            return characteristics_;
        }
        double& current_HP() override {
            return characteristics().current_HP;
        }
        double& initiative() override {
            return characteristics().initiative;
        }
        double damage() override {
            return characteristics().damage;
        }
        const std::string& name() override {
            return characteristics().name;
        }
        int amount() override {
            return 1;
        }
        double xp_for_destroy() override { return 0; }
        void make_turn(Game&, Team self_team) override;
        void accumulate_energy();
        void upgrade_school(std::string& school_name);
        void summon_unit(Game& game, const std::string& school_name, const std::string& skill_name, size_t x, size_t y);
        double damage_coefficient(SchoolsTable& table, unit_t enemy) override;
        void take_damage(double damage) override;
        void make_damage(Game& game, unit_t enemy) override;
        void move(Game& game, int x, int y) override;
};

#endif
