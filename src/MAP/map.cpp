
#include "map.h"

#include <iostream>

using namespace AStarCities;

void Map::setGlobalBounds(double minlat, double maxlat, double minlon, double maxlon) {

    minLatitude = minlat;
    maxLatitude = maxlat;
    minLongitude = minlon;
    maxLongitude = maxlon;

    static constexpr double refSize = 1000;

    if (getGlobalWidth() > getGlobalHeight()) {
        localWidth = refSize;
        localHeight = refSize * (getGlobalHeight() / getGlobalWidth());
    } else {
        localWidth = refSize * (getGlobalWidth() / getGlobalHeight());
        localHeight = refSize;
    }

    std::cout << "Map - global dimensions: " << getGlobalWidth() << " " << getGlobalHeight() << std::endl;
    std::cout << "Map - local  dimensions: " << localWidth << " " << localHeight << std::endl;
}

void Map::addRoad(const Road& road) {

    std::vector<std::reference_wrapper<const Node>> nodes;

    for (const Node& node : road.getNodes()) {
        const Node* newNode = addNode(node);
        if (newNode != nullptr) {
            nodes.push_back(*newNode);
        } else {
            std::cerr << "Map - Error: Unable to create new node - id: " << node.getId();
        }
    }

    const auto [roadIterator, success] = roads.insert({road.getId(), road});
    if (success) {
        roadIterator->second.replaceNodes(nodes);
    } else {
        std::cerr << "Map - Error: Unable to insert new road - id: " << road.getId() << std::endl;
    }
}

const Node* Map::addNode(const Node& node) {
    const auto [nodeIterator, success] = nodes.insert({node.getId(), node});
    if (success) {
        nodeIterator->second.setLocalPosition(globalPosToLocal(node.getGlobalPosition()));
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
