#pragma once

#include <ecs/entity/entity.h>
#include <rvr_parser/parser.h>

namespace rvr {
class Scene {
public:
    void LoadScene(const std::string& sceneName);
private:
    void InitUnits(const std::vector<Parser::Unit>& units);
    void InitUnit(const Parser::Unit& unit);
    void InitEntity(const Parser::Unit& unit);
    static Entity* CreateEntity(const std::vector<Parser::Field>& fields, const Parser::Heading& heading);
    void InitComponent(Entity* entity, Parser::Field field);
    void SaveHierarchyInfo(Entity* entity, const Parser::Heading& heading);
    void CreateHierarchy();
    void AttachRitualBehavior();

    std::map<type::EntityId , type::EntityId> parentIdMap_;
    std::set<type::EntityId> ritualIds_;
};
}