#pragma once

#include "roadtype.h"

#include <cstdint>
#include <functional>
#include <string>

namespace AStarCities {

    class Node;

    class Road {

        public:

            Road(uint64_t id, const std::string& name, RoadType type) : id(id), name(name), type(type) {};

            virtual ~Road() = default;

            void setNodes(const std::vector<std::reference_wrapper<const Node>>& nodes);

            [[nodiscard]] uint64_t    getId()       const noexcept { return id; }
            [[nodiscard]] std::string getName()     const noexcept { return name; }
            [[nodiscard]] RoadType    getRoadType() const noexcept { return type; }

            [[nodiscard]] const std::vector<std::reference_wrapper<const Node>>& getNodes() const { return nodes; }

        private:

            uint64_t id;

            std::string name;

            RoadType type;

            std::vector<std::reference_wrapper<const Node>> nodes;

    };
}
