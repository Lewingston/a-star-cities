
#include "node.h"

#include <cmath>

using namespace AStarCities;

double Node::localDistance(const Node& node) const {
    const auto [x1, y1] = getLocalPosition();
    const auto [x2, y2] = node.getLocalPosition();
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

double Node::globalDistance(const Node& node) const {
    const auto [x1, y1] = getGlobalPosition();
    const auto [x2, y2] = node.getGlobalPosition();
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
