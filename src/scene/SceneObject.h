// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <memory>
#include <string>
#include <vector>

class SceneObject
{
    public:
        virtual ~SceneObject();

        const std::string& objectName() const;

        void setObjectName(const std::string& name);
        
    private:
        std::string m_objectName{""};
        std::vector<std::unique_ptr<SceneObject>> m_children;
};