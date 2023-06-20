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
    static Entity* CreateEntity(const Parser::Heading& heading);
    void SaveHierarchyInfo(Entity* entity, const Parser::Heading& heading);
    void CreateHierarchy();
    static void CreateRituals(const std::vector<Parser::Unit>& units);
    static void CreateComponent(Entity* entity, const std::map<std::string, Parser::Field>& fields,
                                ComponentType cType);
    void Checks();

    std::map<type::EntityId , type::EntityId> parentIdMap_;
};
}