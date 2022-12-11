#include "scene.h"
#include "ecs/component/all_components.h"
#include "ecs/ecs.h"
#include "rvr_parser/parser.h"
#include <ecs/entity/entity_factory.h>
#include <ecs/component/component_init.h>

#define INIT_COMPONENT_CASE(TYPE, NUM) case ComponentType::TYPE: componentInit::Init ## TYPE(entity, field); break;

namespace rvr {
void Scene::LoadScene(const std::string &sceneName) {
    // Parse file
    Parser parser(sceneName + ".rvr");
    auto units = parser.Parse();

    // Init each unit
    for (const auto& unit : units)
        InitUnit(unit);

    // Create hierarchy
    for (auto [childId, parentId] : parentIdMap_) {
        auto parent = ECS::Instance()->GetEntity(parentId);
        auto child = ECS::Instance()->GetEntity(childId);
        CHECK_MSG(parent, Fmt("Parent id %d was null", parentId));
        CHECK_MSG(child, Fmt("Child id %d was null", parentId));
        parent->AddChild(child);
    }
}

void Scene::InitUnit(const Parser::Unit& unit) {
    if (unit.heading.headingType == "entity") {
        InitEntity(unit);
    }
    else if (unit.heading.headingType == "resource") {
        // not implemented yet
    }
    else {
        Throw(Fmt("Heading type %s unrecognized", unit.heading.headingType.c_str()));
    }
}

void Scene::InitEntity(const Parser::Unit& unit) {
    // First create entity
    auto entity = CreateEntity(unit.fields, unit.heading);

    // Initialize the entity's components with data based on fields
    for (const auto& field : unit.fields)
        InitComponent(entity, field);

    // Store parent hierarchy information for later
    SaveHierarchyInfo(entity, unit.heading);
}

Entity* Scene::CreateEntity(const std::vector<Parser::Field>& fields, const Parser::Heading& heading) {
    // First collect the component types from the unit
    std::vector<ComponentType> cTypes;
    cTypes.reserve(fields.size());
    for (const auto& field : fields)
        cTypes.push_back(field.cType);

    // Use the component types to construct a new entity
    auto entity = EntityFactory::CreateEntity(cTypes);

    // Get and set entity name, if any
    try {
        entity->SetName(heading.strKeyStrVal.at("name"));
    }
    catch (std::out_of_range& e) {
        Log::Write(Log::Level::Warning, Fmt("[%s] generating default name", entity->GetName().c_str()));
    }

    return entity;
}

void Scene::InitComponent(Entity* entity, Parser::Field field) {
    switch (field.cType) {
        // See implementations in <ecs/component/component_init.cpp>
        COMPONENT_LIST(INIT_COMPONENT_CASE)
    default:
        THROW(Fmt("Component type %s unrecognized", toString(field.cType)))
    }
}

void Scene::SaveHierarchyInfo(Entity* entity, const Parser::Heading& heading) {
    // root has no parent, so just return
    if (entity->id == 0)
        return;

    try {
        parentIdMap_[entity->id] = heading.strKeyNumVal.at("parent");
    }
    catch (std::out_of_range& e) {
        Log::Write(Log::Level::Warning, Fmt("[%s] using root as default parent", entity->GetName().c_str()));
        parentIdMap_[entity->id] = 0;
    }
}


}
