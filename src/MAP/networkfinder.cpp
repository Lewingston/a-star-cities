
#include "networkfinder.h"
#include "map.h"

#include <iostream>

using namespace AStarCities;

NetworkFinder::NetworkFinder(const Map& map) {

    for (const auto& [id, intersection] : map.getIntersections()) {
        networkNodes.insert({id, NetworkNode(intersection)});
    }
}

void NetworkFinder::generateNetworks() {

    // iterate over all intersections
    for (auto& [id, intersection] : networkNodes) {

        // check if the intersection is not allready part of a network
        if (intersection.network != nullptr)
            continue;

        Network& network = networks.emplace_back(Network());
        getNetwork(intersection, network);
    }
}

std::vector<NetworkFinder::RoadNetwork> NetworkFinder::getRoadNetworks() const {
    std::vector<RoadNetwork> networks;
    for (const Network& network : this->networks) {
        auto& roadNetwork = networks.emplace_back(RoadNetwork());
        for (std::reference_wrapper<const NetworkNode> node : network) {
            for (std::reference_wrapper<const Road> road : node.get().intersection.getRoads()) {
                roadNetwork.insert(road.get());
            }
        }
    }
    return networks;
}

std::size_t NetworkFinder::getNetworkNodeCount(const RoadNetwork& network) {

    std::size_t nodeCount = 0;

    for (const Road& road : network) {
        nodeCount += road.getNodes().size();
    }

    return nodeCount;
}

void NetworkFinder::getNetwork(NetworkNode& node, Network& network) {

    // check if the intersection is allready part of the network
    if (network.contains(node))
        return;

    // add this intersection to the network
    network.insert(node);
    node.network = &network;

    // follow the roads
    for (const Road& road : node.intersection.getRoads()) {
        const auto& [start, end] = road.getIntersections();
        NetworkNode* startNode = getNetworkNode(start);
        NetworkNode* endNode = getNetworkNode(end);
        if (*startNode != node) {
            getNetwork(*startNode, network);
        }
        if (*endNode != node) {
            getNetwork(*endNode, network);
        }
    }
}

NetworkFinder::NetworkNode* NetworkFinder::getNetworkNode(const Intersection& intersection) {
    if (auto nodeIter = networkNodes.find(intersection.getId()); nodeIter != networkNodes.end()) {
        return &nodeIter->second;
    } else {
        return nullptr;
    }
}
