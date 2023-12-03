#pragma once

#include "RoadRenderer.h"
#include "BuildingRenderer.h"

#include "SFML/Graphics/Transform.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RenderTexture.hpp"
#include "SFML/Graphics/CircleShape.hpp"

#include <memory>
#include <vector>
#include <set>

namespace sf {
    class RenderWindow;
    class Event;
}

namespace AStarCities {

    class Map;
    class Intersection;
    class Solver;

    class MapRenderer {

        public:

            MapRenderer();
            virtual ~MapRenderer() = default;

            void openWindow(uint32_t width, uint32_t height);

            void setBackgroundColor(sf::Color color) { backgroundColor = color; }
            void setBoundingBoxColor(sf::Color color) { boundingBoxColor = color; }

            void setRoadColor(RoadType type, sf::Color color);
            void setRoadColor(const std::set<RoadType>& types, sf::Color color);
            void setBuildingColor(BuildingType type, sf::Color color);
            void setBuildingColor(const std::set<BuildingType>& types, sf::Color color);

            void setMap(std::shared_ptr<Map> map);
            void setSolver(std::shared_ptr<Solver> solver);

            void runSimulation();

        private:

            void doSolutionStep();

            void createBoundingBox(float width, float height);

            void handleEvents();
            void handleKeyPress(const sf::Event& event);
            void handleMouseWheel(const sf::Event& event);
            void handleMouseButtons(const sf::Event& event);
            void handleMouseMove(const sf::Event& event);

            void drawMap();
            void drawInterchanges();
            void drawInterchange(const Intersection& inter);

            void fadeRoads();

            void translate(float x, float y);

            std::shared_ptr<sf::RenderWindow> window;

            std::shared_ptr<Map> map;
            std::shared_ptr<Solver> solver;

            std::map<uint64_t, RoadRenderer> roads;
            std::vector<BuildingRenderer> buildings;

            sf::Transform globalTransform;

            std::vector<sf::Vertex> boundingBox;

            bool showBuildings    = true;
            bool showRoads        = true;
            bool showBoundingBox  = false;
            bool showInterchanges = false;

            uint32_t resWidth;
            uint32_t resHeight;

            bool dragMouse = false;
            int oldX = 0;
            int oldY = 0;
            float zoom = 1.0f;

            sf::Color backgroundColor  = sf::Color(10, 10, 10);
            sf::Color boundingBoxColor = sf::Color(255, 0, 0);

            std::map<RoadType, sf::Color> roadColorMap;
            std::map<BuildingType, sf::Color> buildingColorMap;

            std::set<std::reference_wrapper<RoadRenderer>, std::less<RoadRenderer>> whiteRoads;

            static constexpr float INTERSECTION_MARKER_SIZE = 1;

            sf::CircleShape intersectionCircle{INTERSECTION_MARKER_SIZE};
    };
}
