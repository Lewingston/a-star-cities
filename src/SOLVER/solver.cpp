
#include "solver.h"

#include <iterator>
#include <random>
#include <iostream>

using namespace AStarCities;

std::pair<const Intersection&, const Intersection&> Solver::selectStartAndEndIntersection(std::shared_ptr<Map> map) {

    double minDistance = std::max(map->getLocalWidth(), map->getLocalWidth()) / 2;

    int tries = 0;

    while (true) {

        const Intersection& start = selectRandomIntersection(map);
        const Intersection& end   = selectRandomIntersection(map);

        tries++;

        if (start == end) {
            continue;
        }

        if (minDistance < getDistance(start, end)) {
            std::cout << "Tries: " << tries << "\n";
            return {start, end};
        }

        minDistance *= 0.97;
    }
}

const Intersection& Solver::selectRandomIntersection(std::shared_ptr<Map> map) {

    const std::map<uint64_t, Intersection>& nodes = map->getIntersections();

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distr(0, static_cast<int>(nodes.size()));

    auto iter = nodes.begin();
    int rand = distr(generator);
    std::advance(iter, rand);
    return iter->second;
}

double Solver::getDistance(const Intersection& inter1, const Intersection& inter2) {

    const auto [x1, y1] = inter1.getPosition();
    const auto [x2, y2] = inter2.getPosition();

    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
