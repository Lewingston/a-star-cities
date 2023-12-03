#pragma once

#include "node.h"

#include <vector>
#include <cstdint>
#include <iostream>

namespace AStarCities {

    class Node;
    class Road;

    class Intersection {

        public:

            struct Connection {

                Connection() = delete;
                Connection(const Road& road, const Intersection& inter) :
                    road(road), intersection(inter) {}

                const Road& road;
                const Intersection& intersection;

                void printId() const { std::cout << intersection.getId() << '\n'; }
            };

            Intersection(const Node& node) : node(node) {}
            virtual ~Intersection() = default;

            bool operator==(const Intersection& inter) const { return getId() == inter.getId(); }
            bool operator<(const Intersection& inter) const { return getId() < inter.getId(); }

            void addRoad(const Road& road);

            void removeRoad(const Road& road);
            void removeRoad(uint64_t roadId);

            [[nodiscard]] uint64_t getId() const { return node.getId(); }
            [[nodiscard]] const Node& getNode() const { return node; }

            [[nodiscard]] std::size_t getRoadCount() const { return roads.size(); }
            [[nodiscard]] const std::vector<std::reference_wrapper<const Road>>& getRoads() const { return roads; }
            [[nodiscard]] std::vector<Connection> getConnections() const;

            [[nodiscard]] std::pair<double, double> getPosition() const { return node.getLocalPosition(); }

        private:

            const Node& node;

            std::vector<std::reference_wrapper<const Road>> roads;

    };
}
