#pragma once

#include "MAP/roadtype.h"

#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/Color.hpp"

#include <memory>

namespace sf {
    class RenderTarget;
    class Transform;
    class Shader;
}

namespace AStarCities {

    class Road;

    class RoadRenderer {

        public:

            RoadRenderer(const Road& road, sf::Color color = sf::Color(0, 0, 0));
            virtual ~RoadRenderer() = default;

            void draw(std::shared_ptr<sf::RenderTarget> renderTarget, const sf::Transform& transform);

        private:

            const Road& road;

            sf::VertexArray line;

            [[nodiscard]] sf::Color getColorByType(RoadType type) const;
    };
}
