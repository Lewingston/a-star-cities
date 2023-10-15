#pragma once

#include "SFML/Graphics/VertexArray.hpp"

#include <vector>
#include <memory>

namespace sf {
    class RenderTarget;
    class Transform;
}

namespace AStarCities {

    class Road;

    class RoadRenderer {

        public:

            RoadRenderer(const Road& road);
            virtual ~RoadRenderer() = default;

            void draw(std::shared_ptr<sf::RenderTarget> renderTarget, const sf::Transform& transform);

        private:

            const Road& road;

            sf::VertexArray line;
    };
}
