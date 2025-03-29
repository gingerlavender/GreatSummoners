#ifndef GAME_CELL_HPP
#define GAME_CELL_HPP

#include "CellType.hpp"

class GameCell {
    private:
        CellType type_;
    public:
        GameCell() : type_(LAND) {}
        GameCell(CellType type) : type_(type) {}
        const CellType& type() const { return type_; }
        CellType& type() { return type_; }

};

#endif
