
#include "road.h"

using namespace AStarCities;

void Road::addNode(const Node& node) {
    nodes.push_back(node);
}

void Road::replaceNodes(const std::vector<std::reference_wrapper<const Node>>& nodes) {
    this->nodes = nodes;
}
