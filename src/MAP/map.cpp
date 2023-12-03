
#include "map.h"
#include "intersection.h"

#include <iostream>

using namespace AStarCities;

void Map::setReferenceResolution(uint32_t width, uint32_t height) {
    refWidth = width;
    refHeight = height;
}

void Map::setGlobalBounds(double minlat, double maxlat, double minlon, double maxlon) {

    minLatitude = minlat;
    maxLatitude = maxlat;
    minLongitude = minlon;
    maxLongitude = maxlon;

    const double refWidth = static_cast<double>(this->refWidth);
    const double refHeight = static_cast<double>(this->refHeight);

    if (refWidth / refHeight < getGlobalWidth() / getGlobalHeight()) {
        localWidth = refWidth;
        localHeight = refWidth * (getGlobalHeight() / getGlobalWidth());
    } else {
        localWidth = refHeight * (getGlobalWidth() / getGlobalHeight());
        localHeight = refHeight;
    }

    std::cout << "Map - global dimensions: " << getGlobalWidth() << " " << getGlobalHeight() << std::endl;
    std::cout << "Map - local  dimensions: " << localWidth << " " << localHeight << std::endl;
}

void Map::addRoad(const Road& road) {

    std::vector<std::reference_wrapper<const Node>> nodes = addNodes(road.getNodes());

    const auto [roadIterator, success] = roads.insert({road.getId(), road});
    if (success) {
        roadIterator->second.setNodes(nodes);
        idHandler.updateUsedIds(road.getId());
    } else {
        std::cerr << "Map - Error: Unable to insert new road - id: " << road.getId() << std::endl;
    }
}

void Map::addBuilding(const Building& building) {

    std::vector<std::reference_wrapper<const Node>> nodes = addNodes(building.getNodes());

    std::vector<std::vector<std::reference_wrapper<const Node>>> innerShapes;
    for (const std::vector<std::reference_wrapper<const Node>>& shape : building.getInnerShapeNodes()) {
        innerShapes.push_back(addNodes(shape));
    }

    const auto [buildingIterator, success] = buildings.insert({building.getId(), building});
    if (success) {
        buildingIterator->second.setNodes(nodes);
        buildingIterator->second.setInnerShapeNodes(innerShapes);
        idHandler.updateUsedIds(building.getId());
    } else {
        std::cerr << "Map - Error: Unable to insert new building - id: " << building.getId() << std::endl;
    }
}

std::vector<std::reference_wrapper<const Node>> Map::addNodes(const std::vector<std::reference_wrapper<const Node>>& nodes) {

    std::vector<std::reference_wrapper<const Node>> returnNodes;

    for (const Node& node : nodes) {
        const Node* newNode = addNode(node);
        if (newNode != nullptr) {
            returnNodes.push_back(*newNode);
        } else {
            std::cerr << "Map - Error: Unable to create new node - id: " << node.getId();
        }
    }

    return returnNodes;
}

const Node* Map::addNode(const Node& node) {
    const auto [nodeIterator, success] = nodes.insert({node.getId(), node});
    if (success) {
        nodeIterator->second.setLocalPosition(globalPosToLocal(node.getGlobalPosition()));
        idHandler.updateUsedIds(node.getId());
        return &nodeIterator->second;
    } else {
        auto nodeIterator = nodes.find(node.getId());
        if (nodeIterator != nodes.end()) {
            return &nodeIterator->second;
        } else {
            std::cerr << "Map - Error: Unable to insert node and unable to find node - id: " << node.getId() << std::endl;
        }
    }
    return nullptr;
}

std::pair<double, double> Map::globalPosToLocal(std::pair<double, double> globalPos) {
    const double posX = (globalPos.second - minLongitude) / getGlobalWidth()  * localWidth;
    const double posY = localHeight - (globalPos.first  - minLatitude)  / getGlobalHeight() * localHeight;
    return {posX, posY};
}

void Map::analyseRoadNetwork() {

    findIntersections();
    splitRoadsOnIntersections();
    findIntersections();
    fuseRoads(); // this step is optional
    setIntersectionsToEndOfRoads();

    networkFinder = std::unique_ptr<NetworkFinder>(new NetworkFinder(*this));
    networkFinder->generateNetworks();
}

std::unique_ptr<Map> Map::getMainNetwork() const {

    if (!networkFinder)
        return nullptr;

    std::vector<NetworkFinder::RoadNetwork> networks = networkFinder->getRoadNetworks();

    std::unique_ptr<Map> map = std::unique_ptr<Map>(new Map());

    NetworkFinder::RoadNetwork& mainNetwork = networks.at(0);

    std::size_t totalNodeCount = NetworkFinder::getNetworkNodeCount(mainNetwork);

    for (auto iterator = networks.begin() + 1; iterator != networks.end(); iterator++) {
        totalNodeCount += NetworkFinder::getNetworkNodeCount(*iterator);
        if (iterator->size() > mainNetwork.size()) {
            mainNetwork = *iterator;
        }
    }

    map->setGlobalBounds(minLatitude, maxLatitude, minLongitude, maxLongitude);
    for (const Road& road : mainNetwork) {
        map->addRoad(road);
    }

    const std::size_t mainNetworkNodeCount = NetworkFinder::getNetworkNodeCount(mainNetwork);
    const float remainingNodesPercent = static_cast<float>(mainNetworkNodeCount) / static_cast<float>(totalNodeCount) * 100.0f;
    std::cout << "Nodes removed: " << totalNodeCount - mainNetworkNodeCount << " (" << 100.0f - remainingNodesPercent << "%)\n";

    if (remainingNodesPercent < 0.66f) {
        std::cerr << "Mpa - Warning: More then 1/3 of nodes removed from main network. Maybe there is a problem.\n";
    }

    map->analyseRoadNetwork();

    return map;
}

void Map::findIntersections() {

    // all road nodes
    std::map<uint64_t, Intersection> allRoadNodes;
    for (auto& [roadId, road] : roads) {
        for (const Node& node : road.getNodes()) {
            Intersection intersection(node);
            auto [intersectionIterator, success] = allRoadNodes.insert({node.getId(), intersection});
            intersectionIterator->second.addRoad(road);
        }
    }

    // find intersections with multiple roads
    for (const auto& [id, intersection] : allRoadNodes) {
        if (intersection.getRoadCount() > 1) {
            intersections.insert({id, intersection});
        }
    }
}

void printRoad(const Road& road) {
    std::cout << road.getName() << "\n";
    for (const Node& node : road.getNodes()) {
        std::cout << node.getId() << "\n";
    }
    std::cout << std::endl;
}

void Map::splitRoadsOnIntersections () {

    std::map<uint64_t, Road> newRoads;

    // iterate over all roads
    for (const auto& [id, road] : roads) {

        // iterate over all nodes in the road except the first and the last
        const std::vector<std::reference_wrapper<const Node>>& nodes = road.getNodes();
        auto lastIntersection = nodes.begin();
        for (auto nodeIter = nodes.begin() + 1; nodeIter != nodes.end() - 1; nodeIter++) {

            // check if node is intersection
            if (intersections.contains(nodeIter->get().getId())) {

                // create new road from previous intersection to current intersection
                Road newRoad(idHandler.getNewId(), road.getName(), road.getType());
                newRoad.setNodes({lastIntersection, nodeIter + 1});
                const auto [iter, success] = newRoads.insert({newRoad.getId(), newRoad});
                if (!success)
                    std::cerr << "Map - Unable to insert new road: " << newRoad.getName() << " - " << newRoad.getId() << std::endl;
                lastIntersection = nodeIter;
            }
        }

        // create new road for last part of the road
        Road newRoad(road.getId(), road.getName(), road.getType());
        newRoad.setNodes({lastIntersection, nodes.end()});
        const auto [iter, success] = newRoads.insert({newRoad.getId(), newRoad});
        if (!success)
            std::cerr << "Map - Unable to insert new road: " << newRoad.getName() << " - " << newRoad.getId() << std::endl;
    }

    // because of new roads the intersections are no longer valid
    intersections.clear();
    roads = newRoads;
}

/*
 * Remove intersections that have exactly two connected roads
 */
void Map::fuseRoads() {

    // iterate over all intersections
    for (auto& [id, intersection] : intersections) {
        if (intersection.getRoadCount() == 2) {

            const Road& road1 = intersection.getRoads().at(0);
            const Road& road2 = intersection.getRoads().at(1);

            if (road1.getId() != road2.getId() &&
                road1.getType() == road2.getType() &&
                road1.getName() == road2.getName()) {

                // create one new road
                Road newRoad = connectRoads(road1, road2);

                const auto& [newRoadIter, success] = roads.insert({newRoad.getId(), newRoad});
                if (!success)
                    std::cerr << "Map - Failed to create new road" << std::endl;

                // replace road 1 on the other intersection
                uint64_t intersection1Id = road1.getNodes().back().get().getId() == intersection.getId() ?
                    road1.getNodes().front().get().getId() : road1.getNodes().back().get().getId();
                if (auto findInter = intersections.find(intersection1Id); findInter != intersections.end()) {
                    findInter->second.removeRoad(road1.getId());
                    findInter->second.addRoad(newRoadIter->second);
                }

                // replace road 2 on the other intersection
                uint64_t intersection2Id = road2.getNodes().back().get().getId() == intersection.getId() ?
                    road2.getNodes().front().get().getId() : road2.getNodes().back().get().getId();
                if (auto findInter = intersections.find(intersection2Id); findInter != intersections.end()) {
                    findInter->second.removeRoad(road2.getId());
                    findInter->second.addRoad(newRoadIter->second);
                }

                // remove roads from map
                roads.erase(road1.getId());
                roads.erase(road2.getId());

                intersection.removeRoad(road1.getId());
                intersection.removeRoad(road2.getId());
            }
        }
    }

    // remove intersections with no roads
    for (auto iter = intersections.begin(); iter != intersections.end();) {
        if (iter->second.getRoadCount() == 0) {
            iter = intersections.erase(iter);
        } else {
            iter++;
        }
    }
}

Road Map::connectRoads(const Road& road1, const Road& road2) {

    Road road(idHandler.getNewId(), road1.getName(), road1.getType());

    std::vector<std::reference_wrapper<const Node>> newNodes;
    newNodes.reserve(road1.getNodes().size() + road2.getNodes().size() - 1);

    if (road1.getNodes().front().get().getId() == road2.getNodes().front().get().getId()) {
        newNodes.insert(newNodes.end(), road1.getNodes().rbegin(), road1.getNodes().rend() - 1);
        newNodes.insert(newNodes.end(), road2.getNodes().begin(), road2.getNodes().end());
    } else if (road1.getNodes().front().get().getId() == road2.getNodes().back().get().getId()) {
        newNodes.insert(newNodes.end(), road1.getNodes().rbegin(), road1.getNodes().rend());
        newNodes.insert(newNodes.end(), road2.getNodes().rbegin() + 1, road2.getNodes().rend());
    } else if (road1.getNodes().back().get().getId() == road2.getNodes().front().get().getId()) {
        newNodes.insert(newNodes.end(), road1.getNodes().begin(), road1.getNodes().end());
        newNodes.insert(newNodes.end(), road2.getNodes().begin() + 1, road2.getNodes().end());
    } else if (road1.getNodes().back().get().getId() == road2.getNodes().back().get().getId()) {
        newNodes.insert(newNodes.end(), road1.getNodes().begin(), road1.getNodes().end());
        newNodes.insert(newNodes.end(), road2.getNodes().rbegin() + 1, road2.getNodes().rend());
    } else {
        std::cerr << "Map - Error: unable to connect roads: " << road1.getId() << " and " << road2.getId() << std::endl;
    }

    road.setNodes(newNodes);

    return road;
}

/*
 * Set intersections at the end of roads. If the end of a road has no intersection
 * because there are no other roads connected, create a new intersection.
 */
void Map::setIntersectionsToEndOfRoads() {

    // iterate over all roads
    for (auto& [id, road] : roads) {

        // check if the road has an intersection at the beginning
        const Node& startNode = road.getStartNode();
        if (auto interIter = intersections.find(startNode.getId()); interIter == intersections.end()) {
            const auto [iter, success] = intersections.insert({startNode.getId(), Intersection(startNode)});
            iter->second.addRoad(road);
            road.setEndPoint(&iter->second);
        } else {
            interIter->second.addRoad(road);
            road.setEndPoint(&interIter->second);
        }

        // check if the road has an intersection at the end
        const Node& endNode = road.getEndNode();
        if (auto interIter = intersections.find(endNode.getId()); interIter == intersections.end()) {
            const auto [iter, success] = intersections.insert({endNode.getId(), Intersection(endNode)});
            iter->second.addRoad(road);
            road.setEndPoint(&iter->second);
        } else {
            interIter->second.addRoad(road);
            road.setEndPoint(&interIter->second);
        }
    }
}
