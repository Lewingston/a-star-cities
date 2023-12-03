#pragma once

#include "MAP/map.h"

#include <limits>

namespace AStarCities {

    class Solver {

        public:

            Solver(std::shared_ptr<Map> map, const Intersection& startNode, const Intersection& endNode);

            virtual ~Solver() = default;

            static std::pair<const Intersection&, const Intersection&> selectStartAndEndIntersection(std::shared_ptr<Map> map);

            const Intersection& getStart() const { return startNode; }
            const Intersection& getEnd()   const { return endNode; }

            void doStep(bool doSubSteps = true);
            Road const* doSubStep();

            [[nodiscard]] bool isDone() const { return solved; }

            [[nodiscard]] std::vector<std::reference_wrapper<const Road>> getSolution() const;

            [[nodiscard]] std::size_t getOpenListSize() const { return openList.size(); }

            void printOpenList();

            [[nodiscard]] std::vector<std::reference_wrapper<const Intersection>> getOpenListIntersections() const;

        private:

            class PathNode {

                public:

                    PathNode(const Intersection& inter) :
                        intersection(inter) {}

                    virtual ~PathNode() = default;

                    bool operator<(const PathNode& node) const {
                        if (node.getIntersection() == getIntersection()) {
                            return false;
                        } if (node.getScore() == getScore()) {
                            return node.getIntersection() < getIntersection();
                        } else {
                            return node.getScore() > getScore();
                        }
                    }

                    [[nodiscard]] uint64_t getId() const { return intersection.getId(); }

                    [[nodiscard]] const Intersection& getIntersection() const { return intersection; }
                    [[nodiscard]] Road const* getRoadToPredecessor() const { return roadToPrev; }
                    [[nodiscard]] PathNode const* getPredecessor() const { return prevNode; }

                    [[nodiscard]] double getDistanceToTarget() const { return distanceToTarget; }
                    [[nodiscard]] double getDistanceTraveled() const { return distanceTraveled; }

                    [[nodiscard]] double getScore() const { return getDistanceToTarget() + getDistanceTraveled(); }

                    void setDistanceToTarget(double distance) { distanceToTarget = distance; }
                    void setDistanceTraveled(double distance) { distanceTraveled = distance; }

                    void setPredecessor(const PathNode& prev, const Road& road) {
                        prevNode = &prev;
                        roadToPrev = &road;
                    }

                    [[nodiscard]] std::vector<Intersection::Connection> getConnections() const { return intersection.getConnections(); }

                private:

                    const Intersection& intersection;
                    PathNode const* prevNode = nullptr;
                    Road const* roadToPrev = nullptr;

                    double distanceTraveled = std::numeric_limits<double>::max();
                    double distanceToTarget = std::numeric_limits<double>::max();
            };

            static const Intersection& selectRandomIntersection(std::shared_ptr<Map> map);

            void init();

            void doSubStep(const PathNode& currentNode, const Intersection::Connection& connection);

            std::shared_ptr<Map> map;

            std::map<uint64_t, PathNode> nodes;

            using PriorityQueue = std::set<std::reference_wrapper<PathNode>, std::less<PathNode>>;

            PriorityQueue openList;
            std::set<std::reference_wrapper<PathNode>, std::less<PathNode>> closedList;

            const Intersection& startNode;
            const Intersection& endNode;

            PathNode* currentNode = nullptr;
            std::vector<Intersection::Connection> currentConnections;
            std::vector<Intersection::Connection>::iterator currentConnectionIterator;

            bool solved = false;

    };
}
