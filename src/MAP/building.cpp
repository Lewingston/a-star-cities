
#include "building.h"

using namespace AStarCities;

void Building::addNode(const Node& node) {
    nodes.push_back(node);
}

void Building::replaceNodes(const std::vector<std::reference_wrapper<const Node>>& nodes) {
    this->nodes = nodes;
}
