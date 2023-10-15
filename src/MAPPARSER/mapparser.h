#pragma once

#include "MAP/map.h"
#include "MAP/node.h"

#include <string>
#include <map>
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

            void parseMap(const std::string& mapData);

            std::shared_ptr<Map> getMap() const { return map; }

        private:

            void parseGlobalBounds(const pugi::xml_document& xml);
            void parseNodes(const pugi::xml_document& xml);
            void parseRoads(const pugi::xml_document& xml);
            void parseRoad(const pugi::xml_node& node);
            std::string getRoadName(const pugi::xml_node& node);

            [[nodiscard]] bool checkIfWayNodeIsHighway(const pugi::xml_node& wayNode) const;
            [[nodiscard]] bool checkHighwayType(const std::string& highwayType) const;

            std::shared_ptr<Map> map;

            std::map<uint64_t, Node> allNodes;

    };
}
