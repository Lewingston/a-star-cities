
#include "intersection.h"
#include "road.h"

using namespace AStarCities;

void Intersection::addRoad(const Road& road) {
    roads.push_back(road);
}

void Intersection::removeRoad(const Road& road) {
    removeRoad(road.getId());
}

void Intersection::removeRoad(uint64_t roadId) {
    roads.erase(std::remove_if(roads.begin(), roads.end(), [roadId](const Road& road){ return road.getId() == roadId; }));
}

std::vector<Intersection::Connection> Intersection::getConnections() const {

    std::vector<Connection> connections;

    for (const Road& road : getRoads()) {

        const auto& [start, end] = road.getIntersections();

        if (*this == start) {
            connections.push_back({road, end});
        } else if (*this == end) {
            connections.push_back({road, start});
        }
    }

    return connections;
}
