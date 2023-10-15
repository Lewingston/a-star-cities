
#include "buildingrenderer.h"

#include "MAP/node.h"
#include "MAP/building.h"

#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Transform.hpp"

using namespace AStarCities;

BuildingRenderer::BuildingRenderer(const Building& building) :
    building(building) {

    const std::vector<std::reference_wrapper<const Node>>& nodes = building.getNodes();
    shape.setPointCount(nodes.size() - 1);
    for (std::size_t ii = 0; ii < nodes.size() - 1; ii++) {
        const auto [posX, posY] = nodes.at(ii).get().getLocalPosition();
        shape.setPoint(ii, sf::Vector2f(static_cast<float>(posX), static_cast<float>(posY)));
    }
    shape.setFillColor(sf::Color(128, 128, 128));
}

void BuildingRenderer::draw(std::shared_ptr<sf::RenderTarget> renderTarget, const sf::Transform& transform) {
    renderTarget->draw(shape, transform);
}
