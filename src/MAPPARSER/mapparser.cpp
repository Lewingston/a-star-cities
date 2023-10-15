
#include "mapparser.h"

#include "pugixml.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <algorithm>

using namespace AStarCities;

std::string MapParser::loadFromFile(const std::string& filePath) const {

    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        std::cerr << "Failed to read file " << filePath << std::endl;
        return "";
    }

    std::ostringstream stringStream;
    stringStream << fileStream.rdbuf();
    return stringStream.str();
}

void MapParser::parseMap(const std::string& mapData) {

    map = std::shared_ptr<Map>(new Map());

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(mapData.c_str());
    if (!result) {
        std::cerr << "Failed to parse xml" << std::endl;
        return;
    }

    parseGlobalBounds(doc);

    parseNodes(doc);

    parseRoads(doc);

    std::cout << "Map node count: " << map->getNodes().size() << std::endl;
    std::cout << "Map road count: " << map->getRoads().size() << std::endl;



}

void MapParser::parseGlobalBounds(const pugi::xml_document& xml) {
    pugi::xml_node boundsNode = xml.child("osm").child("bounds");
    const double minlat = boundsNode.attribute("minlat").as_double();
    const double minlon = boundsNode.attribute("minlon").as_double();
    const double maxlat = boundsNode.attribute("maxlat").as_double();
    const double maxlon = boundsNode.attribute("maxlon").as_double();
    map->setGlobalBounds(minlat, maxlat, minlon, maxlon);
    //map->setGlobalBounds(48.1,48.2,11.5,11.6);
}

void MapParser::parseNodes(const pugi::xml_document& xml) {
    for (pugi::xml_node node : xml.child("osm").children("node")) {
        uint64_t id = node.attribute("id").as_ullong();
        double lat = node.attribute("lat").as_double();
        double lon = node.attribute("lon").as_double();
        allNodes.insert({id, Node(id, lat, lon)});
    }
    std::cout << "All nodes count: " << allNodes.size() << std::endl;
}

void MapParser::parseRoads(const pugi::xml_document& xml) {
    for (pugi::xml_node wayNode : xml.child("osm").children("way")) {
        if (checkIfWayNodeIsHighway(wayNode)) {
            parseRoad(wayNode);
        }
    }
}

void MapParser::parseRoad(const pugi::xml_node& node) {

    Road road = Road(node.attribute("id").as_ullong(), getRoadName(node));

    for (pugi::xml_node nodes : node.children("nd")) {
        const uint64_t nodeId = nodes.attribute("ref").as_ullong();
        auto nodeIterator = allNodes.find(nodeId);
        if (nodeIterator != allNodes.end()) {
            road.addNode(nodeIterator->second);
        } else {
            std::cout << "Parser - Error: Unable to find node '" << nodeId << "' for raod: " << road.getId() << std::endl;
        }
    }

    map->addRoad(road);

}

std::string MapParser::getRoadName(const pugi::xml_node& roadNode) {
    for (const pugi::xml_node& node : roadNode.children("tag")) {
        std::string attr = node.attribute("k").as_string();
        if (attr == "name") {
            return node.attribute("v").as_string();
        }
    }
    return "";
}

bool MapParser::checkIfWayNodeIsHighway(const pugi::xml_node& wayNode) const {

    for (pugi::xml_node key : wayNode.children("tag")) {
        const std::string attr = key.attribute("k").as_string();
        if (attr == "highway") {
            const std::string type = key.attribute("v").as_string();
            if (checkHighwayType(type)) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

bool MapParser::checkHighwayType(const std::string& highwayType) const {

    static const std::vector<std::string> types = {
        "motorway",
        "trunk",
        "primary",
        "secondary",
        "tertiary",
        "motorway_link",
        "trunk_link",
        "primary_link",
        "secondary_link",
        "tertiary_link",
        "road",
        "residential",
        "service",
        "living_street", // spielstraße
        "track",         // feldweg
        "pedestrian",
        "cycleway",
        "footway",
        "path",
        "bridleway", // reitweg
        "crossing",
        "platform",
        "steps",
        "construction",
        "bus_stop",
        "corridor",
        "elevator",
        "unclassified"
    };

    static const std::vector<std::string> exclude = {
        /*"steps",
        "path",
        "footway",
        "service",
        "cycleway",
        "pedestrian",
        "platform"*/
    };

    if (std::find(std::begin(types), std::end(types), highwayType) != std::end(types)) {
        if (std::find(std::begin(exclude), std::end(exclude), highwayType) != std::end(exclude)) {
            return false;
        } else {
            return true;
        }
    } else {
        std::cout << "Unknown highway type: " << highwayType << std::endl;
        return false;
    }
}
