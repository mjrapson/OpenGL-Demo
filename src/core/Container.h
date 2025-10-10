// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <memory>
#include <ranges>
#include <string>
#include <unordered_map>

template<typename Asset>
class Container
{
    public:
        void add(const std::string& id, std::unique_ptr<Asset> asset)
        {
            m_data[id] = std::move(asset);
        }

        Asset* get(const std::string& id) const
        {
            return m_data.at(id).get();
        }

        void remove(const std::string& id)
        {
            m_data.erase(id);
        }

        void clear()
        {
            m_data.clear();
        }

        auto view() const
        {
            return m_data | std::views::transform([](auto& pair) {
                return pair.second.get();
            });
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<Asset>> m_data;
};
