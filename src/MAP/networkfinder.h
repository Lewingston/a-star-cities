#pragma once

#include "intersection.h"

#include <set>
#include <vector>
#include <map>
#include <functional>

namespace AStarCities {

    class Map;

    class NetworkFinder {

        public:

            using RoadNetwork = std::set<std::reference_wrapper<const Road>, std::less<Road>>;

            NetworkFinder(const Map& map);
            virtual ~NetworkFinder() = default;

            void generateNetworks();

            [[nodiscard]] std::vector<RoadNetwork> getRoadNetworks() const;

            [[nodiscard]] static std::size_t getNetworkNodeCount(const RoadNetwork& network);

        private:

            struct NetworkNode {

                NetworkNode(const Intersection& intersection) :
                    intersection(intersection) {};

                virtual ~NetworkNode() = default;

                bool operator<(const NetworkNode& node) const { return node.intersection.getId() < intersection.getId(); }

                bool operator!=(const NetworkNode& node) const { return node.intersection.getId() != intersection.getId(); }

                const Intersection& intersection;
                std::set<std::reference_wrapper<const NetworkNode>, std::less<NetworkNode>> const* network = nullptr;
            };

            using Network = std::set<std::reference_wrapper<const NetworkNode>, std::less<NetworkNode>>;

            void getNetwork(NetworkNode& node, Network& network);

            NetworkNode* getNetworkNode(const Intersection& intersection);

            std::map<uint64_t, NetworkNode> networkNodes;
            std::vector<Network> networks;
    };
}
