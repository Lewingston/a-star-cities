
#include "roadrenderer.h"

#include "MAP/node.h"
#include "MAP/road.h"

#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/Transform.hpp"
#include "SFML/Graphics/Shader.hpp"

using namespace AStarCities;

RoadRenderer::RoadRenderer(const Road& road, sf::Color color) :
    road(road),
    line(sf::PrimitiveType::LineStrip, road.getNodes().size()) {

    /*const uint8_t r = static_cast<uint8_t>(std::rand() % 256);
    const uint8_t g = static_cast<uint8_t>(std::rand() % 256);
    const uint8_t b = static_cast<uint8_t>(std::rand() % 256);
    color = sf::Color(r, g, b);*/

    const std::vector<std::reference_wrapper<const Node>>& nodes = road.getNodes();
    for (std::size_t ii = 0; ii < nodes.size(); ii++) {
        const auto [posX, posY] = nodes.at(ii).get().getLocalPosition();
        line[ii].position = sf::Vector2f(static_cast<float>(posX), static_cast<float>(posY));
        line[ii].color = color;
    }
}

void RoadRenderer::setColor(sf::Color color) {
    for (std::size_t ii = 0; ii < line.getVertexCount(); ii++) {
        line[ii].color = color;
    }
}

void RoadRenderer::draw(std::shared_ptr<sf::RenderTarget> renderTarget, const sf::Transform& transform) {
    sf::RenderStates renderStates;
    renderStates.transform = transform;
    renderTarget->draw(line, renderStates);
}
