#pragma once

#include "roadtype.h"

#include <cstdint>
#include <functional>
#include <string>

namespace AStarCities {

    class Node;
    class Intersection;

    class Road {

        public:

            Road(uint64_t id, const std::string& name, RoadType type) : id(id), name(name), type(type) {};

            virtual ~Road() = default;

            bool operator<(const Road& road) const { return road.id < id; }

            void setNodes(const std::vector<std::reference_wrapper<const Node>>& nodes);

            void setEndPoint(Intersection* intersection);

            [[nodiscard]] uint64_t    getId()   const noexcept { return id; }
            [[nodiscard]] std::string getName() const noexcept { return name; }
            [[nodiscard]] RoadType    getType() const noexcept { return type; }

            [[nodiscard]] const Node& getStartNode() const { return nodes.begin()->get(); }
            [[nodiscard]] const Node& getEndNode() const { return (nodes.end() - 1)->get(); }

            [[nodiscard]] double getGlobalLength() const { return globalLength; }
            [[nodiscard]] double getLocalLength()  const { return localLength; }

            [[nodiscard]] std::pair<const Intersection&, const Intersection&> getIntersections() const;

            [[nodiscard]] const std::vector<std::reference_wrapper<const Node>>& getNodes() const { return nodes; }

        private:

            [[nodiscard]] double calculateLength(bool global) const;

            uint64_t id;

            std::string name;

            RoadType type;

            std::vector<std::reference_wrapper<const Node>> nodes;

            double localLength;
            double globalLength;

            Intersection* startIntersection = nullptr;
            Intersection* endIntersection = nullptr;

    };
}
