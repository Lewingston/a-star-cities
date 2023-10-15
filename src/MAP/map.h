#pragma once

#include "node.h"
#include "road.h"
#include "building.h"

#include <map>

namespace AStarCities {

    class Map {

        public:

            Map() = default;
            virtual ~Map() = default;

            void setGlobalBounds(double minlat, double maxlat, double minlon, double maxlon);

            [[nodiscard]] const std::map<uint64_t, Node>&     getNodes()     const noexcept { return nodes; }
            [[nodiscard]] const std::map<uint64_t, Road>&     getRoads()     const noexcept { return roads; }
            [[nodiscard]] const std::map<uint64_t, Building>& getBuildings() const noexcept { return buildings; }

            [[nodiscard]] double getLocalWidth()  const noexcept { return localWidth; }
            [[nodiscard]] double getLocalHeight() const noexcept { return localHeight; }

            void addRoad(const Road& road);
            void addBuilding(const Building& building);

        private:

            [[nodiscard]] double getGlobalWidth()  const noexcept { return maxLongitude - minLongitude; }
            [[nodiscard]] double getGlobalHeight() const noexcept { return maxLatitude  - minLatitude;  }

            [[nodiscard]] std::pair<double, double> globalPosToLocal(std::pair<double, double> globalPos);

            std::vector<std::reference_wrapper<const Node>> addNodes(const std::vector<std::reference_wrapper<const Node>>& nodes);
            const Node* addNode(const Node& node);

            double minLatitude;
            double maxLatitude;
            double minLongitude;
            double maxLongitude;

            double localWidth;
            double localHeight;

            std::map<uint64_t, Node> nodes;
            std::map<uint64_t, Road> roads;
            std::map<uint64_t, Building> buildings;

    };
}
