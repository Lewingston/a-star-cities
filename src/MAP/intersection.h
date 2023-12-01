#pragma once

#include "node.h"

#include <vector>
#include <cstdint>

namespace AStarCities {

    class Node;
    class Road;

    class Intersection {

        public:

            Intersection(const Node& node) : node(node) {}
            virtual ~Intersection() = default;

            bool operator==(const Intersection& inter) const { return getId() == inter.getId(); }

            void addRoad(const Road& road);

            void removeRoad(const Road& road);
            void removeRoad(uint64_t roadId);

            [[nodiscard]] uint64_t getId() const { return node.getId(); }

            [[nodiscard]] std::size_t getRoadCount() const { return roads.size(); }
            [[nodiscard]] const std::vector<std::reference_wrapper<const Road>>& getRoads() const { return roads; }

            [[nodiscard]] std::pair<double, double> getPosition() const { return node.getLocalPosition(); }

        private:

            const Node& node;

            std::vector<std::reference_wrapper<const Road>> roads;

    };
}
