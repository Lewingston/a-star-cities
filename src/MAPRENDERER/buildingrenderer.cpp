
#include "buildingrenderer.h"

#include "MAP/node.h"
#include "MAP/building.h"

#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Transform.hpp"

#include "mapbox/earcut.hpp"

#include <iostream>

using namespace AStarCities;

BuildingRenderer::BuildingRenderer(const Building& building, sf::Color color) :
    building(building) {

    generateShape(color);
}

void BuildingRenderer::draw(std::shared_ptr<sf::RenderTarget> renderTarget, const sf::Transform& transform) {
    renderTarget->draw(shape, transform);
    for (const auto& shape : innerShapes)
        renderTarget->draw(shape, transform);
}

void BuildingRenderer::generateShape(sf::Color color) {

    std::vector<std::vector<std::array<double, 2>>> points;

    // first add outer shape
    points.push_back({});
    const std::vector<std::reference_wrapper<const Node>>& nodes = building.getNodes();
    for (std::size_t ii = 0; ii < nodes.size() - 1; ii++) {
        const auto [posX, posY] = nodes.at(ii).get().getLocalPosition();
        points.at(0).push_back({posX, posY});
    }

    // add optional inner shapes
    for (const std::vector<std::reference_wrapper<const Node>>& shape : building.getInnerShapeNodes()) {
        points.push_back({});
        std::vector<std::array<double, 2>> innerShape;
        for (std::size_t ii = 0; ii < shape.size() - 1; ii++) {
            const auto [posX, posY] = shape.at(ii).get().getLocalPosition();
            points.back().push_back({posX, posY});
        }
    }

    // run tessellation
    std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(points);

    shape.resize(indices.size());
    shape.setPrimitiveType(sf::PrimitiveType::Triangles);

    // create vertex array
    for (std::size_t ii = 0; ii < indices.size(); ii++) {

        std::size_t indexX = 0;
        std::size_t indexY = indices.at(ii);

        while (indexY >= points.at(indexX).size()) {
            indexY -= points.at(indexX).size();
            indexX++;
        }

        const auto [posX, posY] = points.at(indexX).at(indexY);
        shape[ii].position = sf::Vector2f(static_cast<float>(posX), static_cast<float>(posY));
        shape[ii].color = color;
    }

}
