/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "world/Behaviour.h"

#include <memory>
#include <vector>

class Behaviour;

struct BehaviourComponent
{
    std::vector<std::unique_ptr<Behaviour>> behaviours;

    BehaviourComponent& addBehaviour(std::unique_ptr<Behaviour> behaviour)
    {
        behaviours.push_back(std::move(behaviour));
        return *this;
    }
};
