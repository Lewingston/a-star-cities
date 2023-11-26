
#include "road.h"

#include <iostream>

using namespace AStarCities;

void Road::setNodes(const std::vector<std::reference_wrapper<const Node>>& nodes) {
    this->nodes = nodes;
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
