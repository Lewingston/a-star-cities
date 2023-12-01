#pragma once

#include "MAP/map.h"

namespace AStarCities {

    class Solver {

        public:

            Solver(std::shared_ptr<Map> map, const Node& startNode, const Node& endNode) :
                map(map), startNode(startNode), endNode(endNode) {}

            virtual ~Solver() = default;

            static std::pair<const Intersection&, const Intersection&> selectStartAndEndIntersection(std::shared_ptr<Map> map);

        private:

            static const Intersection& selectRandomIntersection(std::shared_ptr<Map> map);

            static double getDistance(const Intersection& iter1, const Intersection& iter2);

            std::shared_ptr<Map> map;

            const Node& startNode;
            const Node& endNode;


    };
}
