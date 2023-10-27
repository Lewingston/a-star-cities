
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

    //const sf::Color color = getColorByType(road.getType());

    const std::vector<std::reference_wrapper<const Node>>& nodes = road.getNodes();
    for (std::size_t ii = 0; ii < nodes.size(); ii++) {
        const auto [posX, posY] = nodes.at(ii).get().getLocalPosition();
        line[ii].position = sf::Vector2f(static_cast<float>(posX), static_cast<float>(posY));
        line[ii].color = color;
    }
}

void RoadRenderer::draw(std::shared_ptr<sf::RenderTarget> renderTarget, const sf::Transform& transform) {
    sf::RenderStates renderStates;
    renderStates.transform = transform;
    //renderStates.shader    = &shader;
    renderTarget->draw(line, renderStates);
}

/*sf::Color RoadRenderer::getColorByType(RoadType type) const {

    const uint8_t r = std::rand() % 255;
    const uint8_t g = std::rand() % 255;
    const uint8_t b = std::rand() % 255;

    return sf::Color(r, g, b);

    switch (type.getEnumValue()) {
        case  RoadType::LIVING_STREET:
            return sf::Color(16, 16, 16);
        case RoadType::RESIDENTIAL:
            return sf::Color(18, 18, 18);
        case RoadType::UNCLASSIFIED:
            return sf::Color(20, 20, 20);
        case RoadType::TERTIARY:
        case RoadType::TERTIARY_LINK:
            return sf::Color(22, 22, 22);
        case RoadType::SECONDARY:
        case RoadType::SECONDARY_LINK:
            return sf::Color(24, 24, 24);
        case RoadType::PRIMARY:
        case RoadType::PRIMARY_LINK:
            return sf::Color(26, 26, 26);
        case RoadType::TRUNK:
        case RoadType::TRUNK_LINK:
            return sf::Color(28, 28, 28);
        case RoadType::MOTORWAY:
        case RoadType::MOTORWAY_LINK:
            return sf::Color(30, 30, 30);
        default:
            return sf::Color(255, 0, 255);
    }

    return sf::Color(255, 0, 255);
}*/
