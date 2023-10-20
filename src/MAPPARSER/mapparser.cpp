
#include "mapparser.h"

#include "pugixml.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <algorithm>
#include <set>

using namespace AStarCities;

std::string MapParser::loadFromFile(const std::string& filePath) const {

    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        std::cerr << "Parser - Failed to read file " << filePath << std::endl;
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
        std::cerr << "Parser - Failed to parse xml" << std::endl;
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

    for (const pugi::xml_node& node : xml.child("osm").children("way")) {
        if (checkIfXmlNodeIsHighway(node)) {
            parseRoad(node);
        }/* else if (checkIfXmlNodeIsBuilding(node)) {
            parseBuilding(node);
        } else {
            parseOtherWay(node);
        }*/
    }

    /*for (const pugi::xml_node& node : xml.child("osm").children("relation")) {
        if (checkIfXmlNodeIsBuilding(node)) {
            if (checkIfBuildingHasMultipleOuterNodes(node)) {
                if (checkIfBuildingHasNoInnerNodes(node)) {
                    parseMultipleBuildings(node);
                } else {
                    std::cerr << "Parser - Building has multiple outer shapes and inner shapes - " << node.attribute("id").as_ullong() << std::endl;
                }
            } else {
                parseComplexBuilding(node);
            }
        }
    }*/
}

void MapParser::parseRoad(const pugi::xml_node& node) {

    static const std::set<RoadType::Type> allowTypes = {
        RoadType::MOTORWAY,
        RoadType::TRUNK,
        RoadType::PRIMARY,
        RoadType::SECONDARY,
        RoadType::TERTIARY,
        RoadType::UNCLASSIFIED,
        RoadType::RESIDENTIAL,
        RoadType::MOTORWAY_LINK,
        RoadType::TRUNK_LINK,
        RoadType::PRIMARY_LINK,
        RoadType::SECONDARY_LINK,
        RoadType::TERTIARY_LINK,
        RoadType::LIVING_STREET
    };

    const uint64_t id = node.attribute("id").as_ullong();
    const RoadType type = getRoadType(node);

    if (allowTypes.contains(type.getType())) {
        Road road = Road(id, getRoadName(node), type);
        road.setNodes(getNodesFromWay(node));
        map->addRoad(road);
    }
}

void MapParser::parseBuilding(const pugi::xml_node& node) {

    Building building = Building(node.attribute("id").as_ullong());

    building.setNodes(getNodesFromWay(node));

    // some buildings can also be the inner shape of an other building
    const auto [iterator, success] = otherWays.insert({building.getId(), building.getNodes()});
    if (!success)
        std::cerr << "Parser - Failed to save building as other way id: " << iterator->first << std::endl;

    map->addBuilding(building);
}

void MapParser::parseMultipleBuildings(const pugi::xml_node& xml) {

    for (const pugi::xml_node& node : xml) {
        const std::string& type = node.attribute("type").as_string();
        const std::string& role = node.attribute("role").as_string();
        const uint64_t refId = node.attribute("ref").as_ullong();
        if (type == "way" && role == "outer") {
            if (auto search = otherWays.find(refId); search != otherWays.end()) {

            } else {
                std::cerr << "Parser - Unable to building reference: " << refId << std::endl;
            }
        }
    }
}

void MapParser::parseComplexBuilding(const pugi::xml_node& node) {

    Building building = Building(node.attribute("id").as_ullong());

    bool outerNodeFound = false;

    for (const pugi::xml_node& refNode : node.children("member")) {
        const std::string type = refNode.attribute("type").as_string();
        if (type == "way") {

            uint64_t refId = refNode.attribute("ref").as_ullong();
            auto wayIterator = otherWays.find(refId);

            if (wayIterator != otherWays.end()) {
                const std::string role = refNode.attribute("role").as_string();
                if (role == "outer" && outerNodeFound == false) {
                    building.setNodes(wayIterator->second);
                    outerNodeFound = true;
                } else if (role == "inner") {
                    building.addInnerShapeNodes(wayIterator->second);
                } else if (outerNodeFound == true) {
                    std::cerr << "Parser - Building (" << building.getId() << ") has multiple outer shapes." << std::endl;
                    return;
                } else {
                    std::cerr << "Parser - Unknown reference node (" << refId << ") role: " << role << std::endl;
                    return;
                }
            } else {
                std::cerr << "Parser - Unable to find other way with id: " << refId << std::endl;
                return;
            }
        }
    }

    if (outerNodeFound == false) {
        std::cerr << " Parser - Building has no outer shape id : " << building.getId() << std::endl;
        return;
    }

    map->addBuilding(building);
}

void MapParser::parseOtherWay(const pugi::xml_node& node) {

    uint64_t wayId = node.attribute("id").as_ullong();

    const auto [iterator, success] = otherWays.insert({wayId, getNodesFromWay(node)});
    if (!success) {
        std::cerr << "Failed to insert new unknown way - id: " << wayId << std::endl;
    }

}

std::string MapParser::getRoadName(const pugi::xml_node& roadNode) const {
    pugi::xml_node nameNode = getXmlNodeByKeyAttribute(roadNode, "name");
    if (!nameNode)
        return "";
    else
        return nameNode.attribute("v").as_string();
}

RoadType MapParser::getRoadType(const pugi::xml_node& roadNode) const {
    pugi::xml_node typeNode = getXmlNodeByKeyAttribute(roadNode, "highway");
    if (!typeNode)
        return RoadType();
    else
        return RoadType(typeNode.attribute("v").as_string());
}

bool MapParser::checkIfXmlNodeIsHighway(const pugi::xml_node& wayNode) const {

    pugi::xml_node highwayTypeNode = getXmlNodeByKeyAttribute(wayNode, "highway");
    if (!highwayTypeNode)
        return false;

    const std::string highwayType = highwayTypeNode.attribute("v").as_string();
    RoadType type{highwayType};
    if (type == RoadType::UNKNOWN) {
        std::cout << "Unknown road type: " << highwayType << std::endl;
    }
    return RoadType(highwayType) != RoadType::UNKNOWN;
}

bool MapParser::checkIfXmlNodeIsBuilding(const pugi::xml_node& buildingNode) const {

    pugi::xml_node buildingTypeNode = getXmlNodeByKeyAttribute(buildingNode, "building");
    if (!buildingTypeNode)
        return false;

    const std::string buildingType = buildingTypeNode.attribute("v").as_string();
    return checkBuildingType(buildingType);
}

std::vector<std::reference_wrapper<const Node>> MapParser::getNodesFromWay(const pugi::xml_node& xmlNode) const {
    std::vector<std::reference_wrapper<const Node>> nodes;
    for (pugi::xml_node node: xmlNode.children("nd")) {
        const uint64_t nodeId = node.attribute("ref").as_ullong();
        auto nodeIterator = allNodes.find(nodeId);
        if (nodeIterator != allNodes.end()) {
            nodes.push_back(nodeIterator->second);
        } else {
            std::cerr << "Parser - Error: Unable to find node '" << nodeId << "' in xml Node: " << xmlNode.attribute("id").as_string() << std::endl;
        }
    }
    return nodes;
}

pugi::xml_node MapParser::getXmlNodeByKeyAttribute(const pugi::xml_node& node, const std::string& key) const {
    for (pugi::xml_node tagNode : node.children("tag")) {
        const std::string keyAttr = tagNode.attribute("k").as_string();
        if (keyAttr == key) {
            return tagNode;
        }
    }
    return pugi::xml_node();
}

bool MapParser::checkIfBuildingHasMultipleOuterNodes(const pugi::xml_node& xml) const {

    int outerShapeCount = 0;

    for (const pugi::xml_node& node : xml.children("member")) {
        const std::string type = node.attribute("type").as_string();
        const std::string role = node.attribute("role").as_string();
        if (type == "way" && role == "outer") {
            outerShapeCount++;
        }
    }

    return outerShapeCount > 1;
}

bool MapParser::checkIfBuildingHasNoInnerNodes(const pugi::xml_node& xml) const {

    for (const pugi::xml_node& node : xml.children("member")) {
        const std::string type = node.attribute("type").as_string();
        const std::string role = node.attribute("role").as_string();
        if (type == "way" && role == "inner") {
            return false;
        }
    }

    return true;
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
