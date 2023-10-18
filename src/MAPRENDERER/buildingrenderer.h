#pragma once

#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/ConvexShape.hpp"

#include <memory>

namespace sf {
    class RenderTarget;
    class Transform;
}

namespace AStarCities {

    class Building;

    class BuildingRenderer {

        public:

            BuildingRenderer(const Building& building);
            virtual ~BuildingRenderer() = default;

            void draw(std::shared_ptr<sf::RenderTarget> renderTarget, const sf::Transform& transform);

        private:

            void generateShape();

            const Building& building;

            sf::VertexArray shape;

            std::vector<sf::ConvexShape> innerShapes;
    };
}
