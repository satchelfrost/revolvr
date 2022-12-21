#include "scene.h"
#include "ecs/component/all_components.h"
#include "ecs/ecs.h"
#include "rvr_parser/parser.h"
#include <ecs/entity/entity_factory.h>
#include <ecs/component/component_factory.h>
#include <ritual_behaviors_all.h>

#define CREATE_COMPONENT_CASE(TYPE, NUM) case ComponentType::TYPE: componentFactory::Create ## TYPE(entity, field); break;
#define INIT_RITUAL_CASE(TYPE, NUM) case game::RitualBehavior::TYPE: ritual->SetImplementation(new TYPE(rId)); break;

namespace rvr {
void Scene::LoadScene(const std::string &sceneName) {
    Log::Write(Log::Level::Info, Fmt("Loading scene %s", sceneName.c_str()));
    Parser parser(sceneName + ".rvr");
    auto units = parser.Parse();
    InitUnits(units);
    CreateHierarchy();
    AttachRitualBehavior();
    Log::Write(Log::Level::Info, Fmt("Loaded scene %s", sceneName.c_str()));
}

void Scene::InitUnits(const std::vector<Parser::Unit> &units) {
    // Init each unit
    for (const auto& unit : units)
        InitUnit(unit);

    // Fill any holes potentially created by scene description
    ECS::Instance()->FillHoles();
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
        CreateComponent(entity, field);

    // Store parent hierarchy information for later
    SaveHierarchyInfo(entity, unit.heading);
}

Entity* Scene::CreateEntity(const std::vector<Parser::Field>& fields, const Parser::Heading& heading) {
    // Use the component types to construct a new entity
    Entity* entity;
    try {
        type::EntityId id = heading.strKeyNumVal.at("id");
        entity = EntityFactory::CreateEntity(id);
    }
    catch (std::out_of_range& e) {
        THROW("Entity did not contain id");
    }

    // Get and set entity name, if any
    try {
        entity->SetName(heading.strKeyStrVal.at("name"));
    }
    catch (std::out_of_range& e) {
        Log::Write(Log::Level::Warning, Fmt("[%s] generating default name", entity->GetName().c_str()));
    }

    return entity;
}

void Scene::CreateComponent(Entity* entity, const Parser::Field& field) {
    switch (field.cType) {
        // See implementations in <ecs/component/component_init.cpp>
        COMPONENT_LIST(CREATE_COMPONENT_CASE)
    default:
        THROW(Fmt("Component type %s unrecognized", toString(field.cType)))
    }

    // Save ritual ids for attachment later
    if (field.cType == ComponentType::Ritual)
        ritualIds_.emplace(entity->id);
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

void Scene::CreateHierarchy() {
    for (auto [childId, parentId] : parentIdMap_) {
        auto parent = ECS::Instance()->GetEntity(parentId);
        auto child = ECS::Instance()->GetEntity(childId);
        CHECK_MSG(parent, Fmt("Parent id %d was null", parentId));
        CHECK_MSG(child, Fmt("Child id %d was null", childId));
        parent->AddChild(child);
    }
}

void Scene::AttachRitualBehavior() {
    for (auto rId : ritualIds_) {
        auto ritual = ECS::Instance()->GetComponent<Ritual>(rId);
        switch (ritual->behavior) {
            RITUAL_BEHAVIORS(INIT_RITUAL_CASE)
            default:
                THROW("Error initializing ritual behavior")
        }
    }
}
}
