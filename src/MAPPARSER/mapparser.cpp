
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

    parseRoadsAndBuildings(doc);

    std::cout << "Map node count:     " << map->getNodes().size() << std::endl;
    std::cout << "Map road count:     " << map->getRoads().size() << std::endl;
    std::cout << "Map building count: " << map->getBuildings().size() << std::endl;
}

void MapParser::parseGlobalBounds(const pugi::xml_document& xml) {
    pugi::xml_node boundsNode = xml.child("osm").child("bounds");
    if (!boundsNode) {
        guessBoundings = true;
        return;
    }
    const double minlat = boundsNode.attribute("minlat").as_double();
    const double minlon = boundsNode.attribute("minlon").as_double();
    const double maxlat = boundsNode.attribute("maxlat").as_double();
    const double maxlon = boundsNode.attribute("maxlon").as_double();
    map->setGlobalBounds(minlat, maxlat, minlon, maxlon);
}

void MapParser::parseNodes(const pugi::xml_document& xml) {
    for (pugi::xml_node node : xml.child("osm").children("node")) {

        const uint64_t id = node.attribute("id").as_ullong();
        const double lat = node.attribute("lat").as_double();
        const double lon = node.attribute("lon").as_double();

        if (lat < minLat) minLat = lat;
        else if (lat > maxLat) maxLat = lat;
        if (lon < minLon) minLon = lon;
        else if (lon > maxLon) maxLon = lon;

        allNodes.insert({id, Node(id, lat, lon)});
    }

    if (guessBoundings) {
        map->setGlobalBounds(minLat, maxLat, minLon, maxLon);
    }

    std::cout << "All nodes count: " << allNodes.size() << std::endl;
}

void MapParser::parseRoadsAndBuildings(const pugi::xml_document& xml) {
    for (pugi::xml_node node : xml.child("osm").children("way")) {
        if (checkIfWayNodeIsHighway(node)) {
            parseRoad(node);
        } else if (checkIfWayNodeIsBuilding(node)) {
            parseBuilding(node);
        }
    }
}

void MapParser::parseRoad(const pugi::xml_node& node) {

    Road road = Road(node.attribute("id").as_ullong(), getRoadName(node));

    // TODO: this code is duplicate; move to separate function
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

void MapParser::parseBuilding(const pugi::xml_node& node) {

    Building building = Building(node.attribute("id").as_ullong());

    // TODO: this code is duplicate; move to separate function
    for (pugi::xml_node nodes: node.children("nd")) {
        const uint64_t nodeId = nodes.attribute("ref").as_ullong();
        auto nodeIterator = allNodes.find(nodeId);
        if (nodeIterator != allNodes.end()) {
            building.addNode(nodeIterator->second);
        } else {
            std::cout << "Parser - Error: Unable to find node '" << nodeId << "' for building: " << building.getId() << std::endl;
        }
    }

    map->addBuilding(building);
}

std::string MapParser::getRoadName(const pugi::xml_node& roadNode) const {
    pugi::xml_node nameNode = getNodeByKeyAttribute(roadNode, "name");
    if (!nameNode)
        return "";
    else
        return nameNode.attribute("v").as_string();
}

bool MapParser::checkIfWayNodeIsHighway(const pugi::xml_node& wayNode) const {

    pugi::xml_node highwayTypeNode = getNodeByKeyAttribute(wayNode, "highway");
    if (!highwayTypeNode)
        return false;

    const std::string highwayType = highwayTypeNode.attribute("v").as_string();
    return checkHighwayType(highwayType);
}

bool MapParser::checkIfWayNodeIsBuilding(const pugi::xml_node& buildingNode) const {

    pugi::xml_node buildingTypeNode = getNodeByKeyAttribute(buildingNode, "building");
    if (!buildingTypeNode)
        return false;

    const std::string buildingType = buildingTypeNode.attribute("v").as_string();
    return checkBuildingType(buildingType);
}

pugi::xml_node MapParser::getNodeByKeyAttribute(const pugi::xml_node& node, const std::string& key) const {
    for (pugi::xml_node tagNode : node.children("tag")) {
        const std::string keyAttr = tagNode.attribute("k").as_string();
        if (keyAttr == key) {
            return tagNode;
        }
    }
    return pugi::xml_node();
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
        "rest_area",
        "steps",
        "construction",
        "bus_stop",
        "corridor",
        "elevator",
        "proposed",
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
        "proposed"
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

bool MapParser::checkBuildingType(const std::string& buildingType) const {

    static const std::vector<std::string> types = {
        "skyscraper",
        "apartments",
        "residential",
        "commercial",
        "supermarket",
        "retail",
        "hotel",
        "office",
        "bank",
        "industrial",
        "government",
        "warehouse",
        "silo",
        "transportation",
        "house",
        "semidetached_house",
        "detached",
        "bungalow",
        "hospital",
        "university",
        "college",
        "school",
        "kindergarten",
        "cathedral",
        "church",
        "chapel",
        "mosque",
        "monastery",
        "synagogue",
        "religious",
        "museum",
        "kiosk",
        "toilets",
        "stadium",
        "sports_centre",
        "sports_hall",
        "swimming_hall",
        "grandstand",
        "dormitory",
        "train_station",
        "fire_station",
        "public_building",
        "public",
        "civic",
        "funeral_hall",
        "tower",
        "bell_tower",
        "bridge",
        "city_gate",
        "gate",
        "gate_house",
        "beer_tent",
        "pavilion",
        "service",
        "hangar",
        "parking",
        "garages",
        "garage",
        "greenhouse",
        "farm",
        "barn",
        "stable",
        "cabin",
        "farm_auxiliary",
        "shed",
        "sty",             // schwienestall
        "allotment_house", // kleine hütte
        "terrace",
        "carport",
        "roof",
        "shelter",
        "hut",
        "booth",
        "gasometer",
        "transformer_tower",
        "electricity",
        "storage",
        "storage_tank",
        "boathouse",
        "houseboat",
        "boat",
        "ship",
        "pylon",
        "container",
        "elevator",
        "stairs",
        "chimney",
        "construction",
        "ruins",
        "historic",
        "collapsed",
        "bunker",
        "yes"
    };

    static const std::vector<std::string> exclude = {

    };

    if (std::find(std::begin(types), std::end(types), buildingType) != std::end(types)) {
        if (std::find(std::begin(exclude), std::end(exclude), buildingType) != std::end(exclude)) {
            return false;
        } else {
            return true;
        }
    } else {
        std::cout << "Unknown building type: " << buildingType << std::endl;
        return false;
    }
}
