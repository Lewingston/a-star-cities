
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
