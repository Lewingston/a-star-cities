#pragma once

#include "MAP/map.h"
#include "MAP/node.h"
#include "MAP/RoadType.h"
#include "MAP/BuildingType.h"

#include <string>
#include <map>
#include <set>
#include <memory>

namespace pugi {
    class xml_node;
    class xml_document;
}

namespace AStarCities {

    class MapParser {

        public:

            MapParser() = default;
            virtual ~MapParser() = default;

            [[nodiscard]] std::string loadFromFile(const std::string& filePath) const;

            void parseMap(const std::string& mapData, uint32_t refWidth = 1600, uint32_t reHeight = 900);

            std::shared_ptr<Map> getMap() const { return map; }

            void parseRoads(bool parseRoads) { this->parseRoadsEnabled = parseRoads; }
            void parseBuildings(bool parseBuildings) { this->parseBuildingsEnabled = parseBuildings; }

            void parseRoadTypes(const std::set<RoadType> types);

        private:

            void parseGlobalBounds(const pugi::xml_document& xml);

            void parseNodes(const pugi::xml_document& xml);

            void parseRoadsAndBuildings(const pugi::xml_document& xml);

            void parseRoad(const pugi::xml_node& node);
            [[nodiscard]] bool checkIfXmlNodeIsHighway(const pugi::xml_node& wayNode) const;
            [[nodiscard]] bool checkHighwayType(const std::string& highwayType) const;
            [[nodiscard]] std::string getRoadName(const pugi::xml_node& roadNode) const;
            [[nodiscard]] RoadType getRoadType(const pugi::xml_node& roadNode) const;

            void parseBuilding(const pugi::xml_node& xml);
            void parseMultipleBuildings(const pugi::xml_node& xml);
            void parseComplexBuilding(const pugi::xml_node& xml);
            [[nodiscard]] bool checkIfXmlNodeIsBuilding(const pugi::xml_node& buildingNode) const;
            [[nodiscard]] bool checkIfBuildingHasMultipleOuterNodes(const pugi::xml_node& xml) const;
            [[nodiscard]] bool checkIfBuildingHasNoInnerNodes(const pugi::xml_node& xml) const;
            [[nodiscard]] BuildingType getBuildingType(const pugi::xml_node& buildingNode) const;

            void parseOtherWay(const pugi::xml_node& xml);

            [[nodiscard]] std::vector<std::reference_wrapper<const Node>> getNodesFromWay(const pugi::xml_node& xmlNode) const;

            [[nodiscard]] pugi::xml_node getXmlNodeByKeyAttribute(const pugi::xml_node& node, const std::string& key) const;

            std::shared_ptr<Map> map;

            std::map<uint64_t, Node> allNodes;

            std::map<uint64_t, std::vector<std::reference_wrapper<const Node>>> otherWays;

            std::set<RoadType> allowedRoadTypes;
            std::set<BuildingType> allowedBuildingTypes;

            bool guessBoundings = false;

            bool parseRoadsEnabled = true;
            bool parseBuildingsEnabled = true;

            double minLat = 90;
            double maxLat = 0;
            double minLon = 180;
            double maxLon = 0;

    };
}
