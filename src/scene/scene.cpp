/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#include "scene/scene.h"
#include "ecs/component/component_hdrs.h"
#include "ecs/ecs.h"
#include "scene/parser.h"
#include "global_context.h"
#include "ecs/component/component_factory.h"
#include "ecs/system/ritual_system.h"
#include "ecs/system/timer_system.h"

namespace rvr {
void Scene::LoadScene(const std::string &sceneName) {
    Log::Write(Log::Level::Info, Fmt("Loading scene %s", sceneName.c_str()));
    Parser parser(sceneName + ".rvr");
    auto units = parser.Parse();
    InitUnits(units);
    CreateHierarchy();
    CreateRituals(units);
    Checks();
    Log::Write(Log::Level::Info, Fmt("Loaded scene %s", sceneName.c_str()));

    // TODO: put this in scene manager once that exists
    system::ritual::Begin();
    system::timer::Start();
}

void Scene::InitUnits(const std::vector<Parser::Unit> &units) {
    // Init each unit
    for (const auto& unit : units)
        InitUnit(unit);

    // Fill any holes potentially created by scene description
    GlobalContext::Inst()->GetECS()->FillHoles();
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
    auto entity = CreateEntity(unit.heading);

    // Create components
    for (const auto& [cType, fields] : unit.fields) {
        // Skip rituals since their constructor might require things existing
        if (cType == ComponentType::Ritual)
            continue;

        CreateComponent(entity, fields, cType);
    }

    // Store parent hierarchy information for later
    SaveHierarchyInfo(entity, unit.heading);
}

Entity* Scene::CreateEntity(const Parser::Heading& heading) {
    // Use the component types to construct a new entity
    Entity* entity;
    try {
        type::EntityId id = heading.strKeyNumVal.at("id");
        entity = GlobalContext::Inst()->GetECS()->CreateNewEntity(id, false);
    }
    catch (std::out_of_range& e) {
        THROW("Entity did not contain id");
    }

    // Get and set entity name, if any
    try {
        entity->SetName(heading.strKeyStrVal.at("name"));
        if (entity->GetName() == "Player" || entity->GetName() == "player")
            GlobalContext::Inst()->PLAYER_ID = entity->id;
    }
    catch (std::out_of_range& e) {
        Log::Write(Log::Level::Warning, Fmt("[%s] generating default name", entity->GetName().c_str()));
    }

    return entity;
}

void Scene::CreateComponent(Entity* entity, const std::map<std::string, Parser::Field>& fields, ComponentType cType) {
    switch (cType) {
        // See implementations in <ecs/component/component_factory.cpp>
        #define COMPONENT_CASE(TYPE) case ComponentType::TYPE: componentFactory::Create ## TYPE(entity, fields); break;
        COMPONENT_LIST(COMPONENT_CASE)
        #undef COMPONENT_CASE
    default:
        THROW(Fmt("Component type %s unrecognized", toString(cType)))
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
        Log::Write(Log::Level::Warning, Fmt("[%s] using root as default parent",
                          entity->GetName().c_str()));
        parentIdMap_[entity->id] = 0;
    }
}

void Scene::CreateHierarchy() {
    for (auto [childId, parentId] : parentIdMap_) {
        auto parent = GlobalContext::Inst()->GetECS()->GetEntity(parentId);
        auto child = GlobalContext::Inst()->GetECS()->GetEntity(childId);
        CHECK_MSG(parent, Fmt("Parent id %d was null", parentId));
        CHECK_MSG(child, Fmt("Child id %d was null", childId));
        parent->AddChild(child);
    }
}

void Scene::CreateRituals(const std::vector<Parser::Unit>& units) {
    for (const auto& unit : units) {
        if (unit.heading.headingType == "entity") {
            for (const auto& [cType, fields] : unit.fields) {
                if (cType == ComponentType::Ritual) {
                    type::EntityId id = unit.heading.strKeyNumVal.at("id");
                    auto entity = GlobalContext::Inst()->GetECS()->GetEntity(id);
                    componentFactory::CreateRitual(entity, fields);
                }
            }
        }
    }
}

void Scene::Checks() {
    if (GlobalContext::Inst()->PLAYER_ID == 0)
        Log::Write(Log::Level::Warning, Fmt("Using root as player entity"));
}
}
