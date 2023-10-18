
#include "building.h"

using namespace AStarCities;

void Building::setNodes(const std::vector<std::reference_wrapper<const Node>>& nodes) {
    this->nodes = nodes;
}

void Building::addInnerShapeNodes(const std::vector<std::reference_wrapper<const Node>>& nodes) {

    innerShapes.push_back(nodes);
}

void Building::setInnerShapeNodes(const std::vector<std::vector<std::reference_wrapper<const Node>>>& shapes) {

    innerShapes = shapes;
}
