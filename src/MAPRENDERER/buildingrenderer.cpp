
#include "buildingrenderer.h"

#include "MAP/node.h"
#include "MAP/building.h"

#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Transform.hpp"

#include "mapbox/earcut.hpp"

using namespace AStarCities;

BuildingRenderer::BuildingRenderer(const Building& building) :
    building(building) {

    generateShape();
}

void BuildingRenderer::draw(std::shared_ptr<sf::RenderTarget> renderTarget, const sf::Transform& transform) {
    renderTarget->draw(shape, transform);
}

void BuildingRenderer::generateShape() {

    std::vector<std::vector<std::array<double, 2>>> points;

    // first add outer shape
    points.push_back({});
    const std::vector<std::reference_wrapper<const Node>>& nodes = building.getNodes();
    for (std::size_t ii = 0; ii < nodes.size() - 1; ii++) {
        const auto [posX, posY] = nodes.at(ii).get().getLocalPosition();
        points.at(0).push_back({posX, posY});
    }

    // add optional inner shapes

    // run tessellation
    std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(points);

    shape.resize(indices.size());
    shape.setPrimitiveType(sf::PrimitiveType::Triangles);

    // create vertex array
    for (std::size_t ii = 0; ii < indices.size(); ii++) {
        const auto [posX, posY] = nodes.at(indices.at(ii)).get().getLocalPosition();
        shape[ii].position = sf::Vector2f(static_cast<float>(posX), static_cast<float>(posY));
        shape[ii].color = sf::Color(128, 128, 128);
    }

}
