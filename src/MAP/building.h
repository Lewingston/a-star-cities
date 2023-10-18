#pragma once

#include <cstdint>
#include <functional>

namespace AStarCities {

    class Node;

    class Building {

        public:

            Building(uint64_t id) : id(id) {};

            Building(uint64_t id, const std::vector<std::reference_wrapper<const Node>>& nodes) :
                id(id), nodes(nodes) {};

            virtual ~Building() = default;

            void setNodes(const std::vector<std::reference_wrapper<const Node>>& nodes);

            void addInnerShapeNodes(const std::vector<std::reference_wrapper<const Node>>& nodes);

            void setInnerShapeNodes(const std::vector<std::vector<std::reference_wrapper<const Node>>>& shapes);

            [[nodiscard]] uint64_t getId() const noexcept { return id; }

            [[nodiscard]] const std::vector<std::reference_wrapper<const Node>>& getNodes() const { return nodes; }

            [[nodiscard]] const std::vector<std::vector<std::reference_wrapper<const Node>>>& getInnerShapeNodes() const { return innerShapes; }

        private:

            uint64_t id;

            std::vector<std::reference_wrapper<const Node>> nodes;

            std::vector<std::vector<std::reference_wrapper<const Node>>> innerShapes;

    };
}
