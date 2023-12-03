
#include "solver.h"

#include <iterator>
#include <random>
#include <iostream>

using namespace AStarCities;

Solver::Solver(std::shared_ptr<Map> map, const Intersection& start, const Intersection& end) :
    map(map), startNode(start), endNode(end) {

    init();
    }

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

        if (minDistance < start.getNode().localDistance(end.getNode())) {
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
    std::uniform_int_distribution<> distr(0, static_cast<int>(nodes.size() - 1));

    auto iter = nodes.begin();
    int rand = distr(generator);
    std::advance(iter, rand);
    return iter->second;
}

void Solver::init() {

    openList.clear();
    closedList.clear();
    nodes.clear();

    // init path nodes
    // TODO: it is ineffective to generate all elements at the start
    // maybe there is a better solution
    for (const auto& [id, inter] : map->getIntersections()) {
        nodes.insert({id, PathNode(inter)});
    }

    // init open list
    PathNode& start = nodes.find(startNode.getId())->second;
    start.setDistanceToTarget(startNode.getNode().localDistance(endNode.getNode()));
    start.setDistanceTraveled(0);
    openList.insert(start);
}

void Solver::doStep(bool doSubSteps) {

    if (solved)
        return;

    if (openList.empty()) {
        // TODO: algorithm allready terminated
        return;
    }

    // get the node with the lowest score from the open list and remove it
    PriorityQueue::iterator iter = openList.begin();
    PathNode& currentNode = iter->get();

    if (currentNode.getIntersection() == endNode) {
        this->currentNode = nullptr;
        solved = true;
        return;
    }

    openList.erase(iter);

    this->currentNode = &currentNode;

    closedList.insert(currentNode);

    currentConnections = currentNode.getConnections();
    currentConnectionIterator = currentConnections.begin();

    if (!doSubSteps)
        return;

    //expandNode(currentNode);

    for (const Intersection::Connection& connection : currentConnections) {
        doSubStep(currentNode, connection);
    }

    if (openList.empty()) {
        // TODO: no solution found
        return;
    }
}

Road const* Solver::doSubStep() {

    if (currentNode == nullptr)
        doStep(false);

    if (solved)
        return nullptr;

    doSubStep(*currentNode, *currentConnectionIterator);

    Road const* road = &currentConnectionIterator->road;

    currentConnectionIterator++;

    if (currentConnectionIterator == currentConnections.end())
        currentNode = nullptr;

    return road;
}

void Solver::doSubStep(const PathNode& currentNode, const Intersection::Connection& connection) {

    PathNode& nextNode = nodes.find(connection.intersection.getId())->second;

    if (closedList.contains(nextNode))
        return;

    double newDistance = connection.road.getLocalLength() + currentNode.getDistanceTraveled();

    if (openList.contains(nextNode) && newDistance >= nextNode.getDistanceTraveled())
        return;

    nextNode.setPredecessor(currentNode, connection.road);
    nextNode.setDistanceTraveled(newDistance);
    const double distanceToTarget = connection.intersection.getNode().localDistance(endNode.getNode());
    nextNode.setDistanceToTarget(distanceToTarget);

    // Elements in a set are constant. Therefor the element has to be removed
    // and reinserted for the key to be updated
    auto [iter, success] = openList.insert(nextNode);
    if (!success) {
        //std::cout << "Update node in open list: " << iter->get().getId() << '\n';
        openList.erase(iter);
        openList.insert(nextNode);
    } else {
        //std::cout << "Enter new node in open list: " << connection.intersection.getId() << '\n';
    }
}

std::vector<std::reference_wrapper<const Road>> Solver::getSolution() const {

    std::vector<std::reference_wrapper<const Road>> solution;

    std::reference_wrapper<const PathNode> currentNode = openList.begin()->get();

    while (currentNode.get().getId() != startNode.getId()) {
        if (currentNode.get().getRoadToPredecessor() == nullptr) {
            std::cerr << "Solver: ERROR - Node hast no road to predecessor.\n";
            break;
        }

        solution.push_back(*currentNode.get().getRoadToPredecessor());

        if (currentNode.get().getPredecessor() == nullptr) {
            std::cerr << "Solver: ERROR - Node hat no predecessor.\n";
        }

        currentNode = *currentNode.get().getPredecessor();
    }

    return solution;
}

void Solver::printOpenList() {
    std::cout << "\nOpen List (" << openList.size() << "):\n";
    for (const PathNode& node : openList) {
        //std::cout << node.getId() << " - " << node.getScore() << '\n';
        std::cout << node.getId() << " - traveled: " << node.getDistanceTraveled() << " - distance: " << node.getDistanceToTarget() << '\n';
    }
    std::cout << '\n';
}

std::vector<std::reference_wrapper<const Intersection>> Solver::getOpenListIntersections() const {

    std::vector<std::reference_wrapper<const Intersection>> intersections;

    for (const PathNode& node : openList) {
        intersections.push_back(node.getIntersection());
    }

    return intersections;
}
