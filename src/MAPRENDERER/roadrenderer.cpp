
#include "roadrenderer.h"

#include "MAP/node.h"
#include "MAP/road.h"

#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/Transform.hpp"

using namespace AStarCities;

RoadRenderer::RoadRenderer(const Road& road) :
    road(road),
    line(sf::PrimitiveType::LineStrip, road.getNodes().size()) {

    const std::vector<std::reference_wrapper<const Node>>& nodes = road.getNodes();
    for (std::size_t ii = 0; ii < nodes.size(); ii++) {
        const auto [posX, posY] = nodes.at(ii).get().getLocalPosition();
        line[ii].position = sf::Vector2f(static_cast<float>(posX), static_cast<float>(posY));
        line[ii].color = sf::Color::Black;
    }
}

void RoadRenderer::draw(std::shared_ptr<sf::RenderTarget> renderTarget, const sf::Transform& transform) {
    renderTarget->draw(line, transform);
}
