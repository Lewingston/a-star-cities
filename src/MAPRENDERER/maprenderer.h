#pragma once

#include "RoadRenderer.h"

#include "SFML/Graphics/Transform.hpp"

#include <memory>
#include <vector>

namespace sf {
    class RenderWindow;
    class Event;
}

namespace AStarCities {

    class Map;

    class MapRenderer {

        public:

            MapRenderer() = default;
            virtual ~MapRenderer() = default;

            void openWindow();

            void setMap(std::shared_ptr<Map> map);

            void runSimulation();

        private:

            void handleEvents();
            void handleKeyPress(const sf::Event& event);
            void handleMouseWheel(const sf::Event& event);
            void handleMouseButtons(const sf::Event& event);
            void handleMouseMove(const sf::Event& event);

            void drawMap();

            void translate(float x, float y);

            std::shared_ptr<sf::RenderWindow> window;

            std::shared_ptr<Map> map;

            std::vector<RoadRenderer> roads;

            sf::Transform globalTransform;

            bool dragMouse = false;
            int oldX = 0;
            int oldY = 0;
            float zoom = 1.0f;
    };
}
