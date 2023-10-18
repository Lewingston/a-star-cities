#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace AStarCities {

    class Node;

    class Road {

        public:

            Road(uint64_t id, const std::string& name) : id(id), name(name) {};

            virtual ~Road() = default;

            void setNodes(const std::vector<std::reference_wrapper<const Node>>& nodes);

            [[nodiscard]] uint64_t getId() const noexcept { return id; }
            [[nodiscard]] std::string getName() const noexcept { return name; }

            [[nodiscard]] const std::vector<std::reference_wrapper<const Node>>& getNodes() const { return nodes; }

        private:

            uint64_t id;

            std::string name;

            std::vector<std::reference_wrapper<const Node>> nodes;

    };
}
