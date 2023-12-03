
#include "road.h"
#include "node.h"

#include <iostream>

using namespace AStarCities;

void Road::setNodes(const std::vector<std::reference_wrapper<const Node>>& nodes) {
    this->nodes = nodes;
    localLength = calculateLength(false);
    globalLength = calculateLength(true);
}

std::pair<const Intersection&, const Intersection&> Road::getIntersections() const {
    return {*startIntersection, *endIntersection};
}

void Road::setEndPoint(Intersection* intersection) {

    if (startIntersection == nullptr) {
        startIntersection = intersection;
    } else if (endIntersection == nullptr) {
        endIntersection = intersection;
    } else {
        std::cerr << "Map - Error: Map end points are allready set. Something went wrong\n";
    }
}

double Road::calculateLength(bool global) const {

    double length = 0;

    auto currentNode = nodes.begin();

    for (auto nextNode = nodes.begin() + 1; nextNode != nodes.end(); nextNode++) {

        if (!global)
            length += currentNode->get().localDistance(*nextNode);
        else
            length += currentNode->get().globalDistance(*nextNode);

        currentNode = nextNode;
    }

    return length;
}
