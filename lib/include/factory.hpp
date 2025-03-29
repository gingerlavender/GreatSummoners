#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "units.hpp"

class Factory {
    public:
        static std::shared_ptr<MoralUnit> create_moral_unit(UnitDescriptor& descriptor) {
            return std::make_shared<MoralUnit>(0, 0, descriptor); 
        }
        static std::shared_ptr<AmoralUnit> create_amoral_unit(UnitDescriptor& descriptor) {
            return std::make_shared<AmoralUnit>(0, 0, descriptor);
        }
        static std::shared_ptr<RessurectionUnit> create_ressurection_unit(UnitDescriptor& descriptor) {
            auto entity = std::make_shared<RessurectionUnit>(0, 0, descriptor);
            if (descriptor.morality == std::nullopt) {
                entity->unit() = std::make_shared<AmoralUnit>(0, 0, descriptor);
            } else {
                entity->unit() = std::make_shared<MoralUnit>(0, 0, descriptor);
            }
            return entity;
        }   
        static std::shared_ptr<Kamikaze> create_kamikaze(UnitDescriptor& descriptor) {
            return std::make_shared<Kamikaze>(0, 0, descriptor);
        }
};

#endif
