#pragma once

#include "RoadRenderer.h"
#include "BuildingRenderer.h"

#include "SFML/Graphics/Transform.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RenderTexture.hpp"

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

            MapRenderer();
            virtual ~MapRenderer() = default;

            void openWindow();

            void setBackgroundColor(sf::Color color) { backgroundColor = color; }
            void setBoundingBoxColor(sf::Color color) { boundingBoxColor = color; }

            void setRoadColor(RoadType type, sf::Color color);
            void setRoadColor(const std::set<RoadType>& types, sf::Color color);
            void setBuildingColor(BuildingType type, sf::Color color);
            void setBuildingColor(const std::set<BuildingType>& types, sf::Color color);

            void setMap(std::shared_ptr<Map> map);

            void runSimulation();

        private:

            void createBoundingBox(float width, float height);

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
            std::vector<BuildingRenderer> buildings;

            sf::Transform globalTransform;

            std::vector<sf::Vertex> boundingBox;

            bool dragMouse = false;
            int oldX = 0;
            int oldY = 0;
            float zoom = 1.0f;

            bool showBuildings   = true;
            bool showRoads       = true;
            bool showBoundingBox = false;

            sf::Color backgroundColor  = sf::Color(10, 10, 10);
            sf::Color boundingBoxColor = sf::Color(255, 0, 0);

            std::map<RoadType, sf::Color> roadColorMap;
            std::map<BuildingType, sf::Color> buildingColorMap;
    };
}
