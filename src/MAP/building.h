#pragma once

#include <cstdint>
#include <functional>

namespace AStarCities {

    class Node;

    class Building {

        public:

            Building(uint64_t id) : id(id) {};

            virtual ~Building() = default;

            void addNode(const Node& node);
            void replaceNodes(const std::vector<std::reference_wrapper<const Node>>& nodes);

            [[nodiscard]] uint64_t getId() const noexcept { return id; }

            [[nodiscard]] const std::vector<std::reference_wrapper<const Node>>& getNodes() const { return nodes; }

        private:

            uint64_t id;

            std::vector<std::reference_wrapper<const Node>> nodes;

    };
}
