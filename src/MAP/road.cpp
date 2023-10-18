
#include "road.h"

using namespace AStarCities;

void Road::setNodes(const std::vector<std::reference_wrapper<const Node>>& nodes) {
    this->nodes = nodes;
}
