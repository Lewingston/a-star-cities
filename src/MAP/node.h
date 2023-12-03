#pragma once

#include <cstdint>
#include <utility>

namespace AStarCities {

    class Node {

        public:

            Node(uint64_t id, double latitude, double longitude) :
                id (id), latitude(latitude), longitude(longitude) {};

            virtual ~Node() = default;

            [[nodiscard]] uint64_t getId() const noexcept { return id; }

            [[nodiscard]] std::pair<double, double> getLocalPosition() const noexcept { return {posX, posY}; }
            [[nodiscard]] std::pair<double, double> getGlobalPosition() const noexcept { return {latitude, longitude}; }

            void setLocalPosition(std::pair<double, double> pos) { posX = pos.first; posY = pos.second; }

            [[nodiscard]] double localDistance(const Node& node) const;
            [[nodiscard]] double globalDistance(const Node& node) const;


        private:

            uint64_t id;

            double latitude;
            double longitude;

            double posX = 0;
            double posY = 0;

    };
}
