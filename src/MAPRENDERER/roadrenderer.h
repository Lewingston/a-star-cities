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

            bool operator<(const RoadRenderer& road) const;

            void draw(std::shared_ptr<sf::RenderTarget> renderTarget, const sf::Transform& transform) const;

            void setColor(sf::Color color);
            void setColor(double color);
            //[[nodiscard]] sf::Color getColor() const { return color; }
            [[nodiscard]] double getColor() const { return color; }

            [[nodiscard]] const Road& getRoad() const { return road; }

        private:

            const Road& road;

            //sf::Color color;

            double color;

            sf::VertexArray line;

            [[nodiscard]] sf::Color getColorByType(RoadType type) const;
    };
}
