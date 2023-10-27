
#include "maprenderer.h"

#include "MAP/map.h"

#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"

#include <iostream>

using namespace AStarCities;

MapRenderer::MapRenderer() {

    // init road color map
    for (RoadType type : RoadType::getAll()) {
        roadColorMap.insert({type, sf::Color(0, 0, 0)});
    }

    // init building color map
    for (BuildingType type : BuildingType::getAll()) {
        buildingColorMap.insert({type, sf::Color(128, 128, 128)});
    }

}

void MapRenderer::openWindow() {

    globalTransform.translate(sf::Vector2f(500, 150));

    window = std::shared_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(sf::Vector2(2000u, 1300u)), "A* Cities"));
    window->setFramerateLimit(60);
    window->clear(sf::Color::White);
    window->display();
}

void MapRenderer::setMap(std::shared_ptr<Map> map) {

    this->map = map;

    for (const auto& pair : map->getRoads()) {
        const Road& road = pair.second;
        roads.push_back(RoadRenderer(road, roadColorMap[road.getType()]));
    }

    for (const auto& pair : map->getBuildings()) {
        const Building& building = pair.second;
        buildings.push_back(BuildingRenderer(building, buildingColorMap[building.getType()]));
    }

    createBoundingBox(static_cast<float>(map->getLocalWidth()), static_cast<float>(map->getLocalHeight()));
}

void MapRenderer::setRoadColor(RoadType type, sf::Color color) {
    if (auto find = roadColorMap.find(type); find != roadColorMap.end()) {
        find->second = color;
    } else {
        std::cerr << "Renderer WARNING: Try to set color of unknown road type: " << type.getEnumValue() << std::endl;
    }
}

void MapRenderer::setRoadColor(const std::set<RoadType>& types, sf::Color color) {
    for (RoadType type : types) {
        setRoadColor(type, color);
    }
}

void MapRenderer::setBuildingColor(BuildingType type, sf::Color color) {
    if (auto find = buildingColorMap.find(type); find != buildingColorMap.end()) {
        find->second = color;
    } else {
        std::cerr << "Renderer WARNING: Try to set color of unknown building type: " << type.getEnumValue() << std::endl;
    }
}

void MapRenderer::setBuildingColor(const std::set<BuildingType>& types, sf::Color color) {
    for (BuildingType type : types) {
        setBuildingColor(type, color);
    }
}

void MapRenderer::createBoundingBox(float width, float height) {
    boundingBox.clear();
    boundingBox.push_back(sf::Vertex(sf::Vector2f(0, 0),          boundingBoxColor));
    boundingBox.push_back(sf::Vertex(sf::Vector2f(width, 0),      boundingBoxColor));
    boundingBox.push_back(sf::Vertex(sf::Vector2f(width, height), boundingBoxColor));
    boundingBox.push_back(sf::Vertex(sf::Vector2f(0, height),     boundingBoxColor));
    boundingBox.push_back(sf::Vertex(sf::Vector2f(0, 0),          boundingBoxColor));
}

void MapRenderer::runSimulation() {

    while (window->isOpen()) {

        handleEvents();

        drawMap();
    }
}

void MapRenderer::handleEvents() {
    sf::Event event;
    while(window->pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::KeyPressed:
                handleKeyPress(event);
                break;
            case sf::Event::MouseWheelScrolled:
                handleMouseWheel(event);
                break;
            case sf::Event::MouseButtonReleased:
            case sf::Event::MouseButtonPressed:
                handleMouseButtons(event);
                break;
            case sf::Event::MouseMoved:
                handleMouseMove(event);
                break;
            default:
                break;
        }
    }
}

void MapRenderer::handleKeyPress(const sf::Event& event) {
    switch (event.key.code) {
        case sf::Keyboard::Enter:
            break;
        case sf::Keyboard::R:
        case sf::Keyboard::S:
            showRoads = !showRoads;
            break;
        case sf::Keyboard::B:
            showBuildings = !showBuildings;
            break;
        default:
            break;
    }
}

void MapRenderer::handleMouseWheel(const sf::Event& event) {

    static constexpr float zoomIn = 1.2f;
    static constexpr float zoomOut = 1.0f / zoomIn;

    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {

        const float mouseX = static_cast<float>(event.mouseWheelScroll.x);
        const float mouseY = static_cast<float>(event.mouseWheelScroll.y);

        const sf::Transform invert = globalTransform.getInverse();
        const sf::Vector2f mouseVector = invert.transformPoint(sf::Vector2f(mouseX, mouseY));

        if (event.mouseWheelScroll.delta > 0) {
            globalTransform.scale(sf::Vector2f(zoomIn, zoomIn), mouseVector);
            zoom *= zoomIn;
        } else if (event.mouseWheelScroll.delta < 0) {
            globalTransform.scale(sf::Vector2f(zoomOut, zoomOut), mouseVector);
            zoom *= zoomOut;
        }
    }
}

void MapRenderer::handleMouseButtons(const sf::Event& event) {
    if (event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonPressed) {
        dragMouse = true;
        oldX = event.mouseButton.x;
        oldY = event.mouseButton.y;
    } else if (event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonReleased) {
        dragMouse = false;
    }
}

void MapRenderer::handleMouseMove(const sf::Event& event) {

    if (!dragMouse)
        return;

    const float x = static_cast<float>(event.mouseMove.x - oldX) / zoom;
    const float y = static_cast<float>(event.mouseMove.y - oldY) / zoom;

    globalTransform.translate(sf::Vector2f(x, y));

    oldX = event.mouseMove.x;
    oldY = event.mouseMove.y;
}

void MapRenderer::drawMap() {

    window->clear(backgroundColor);

    if (showRoads) {
        for (RoadRenderer& road : roads) {
            road.draw(window, globalTransform);
        }
    }

    if (showBuildings) {
        for (BuildingRenderer& building : buildings) {
            building.draw(window, globalTransform);
        }
    }

    if (showBoundingBox) {
        window->draw(&boundingBox[0], boundingBox.size(), sf::PrimitiveType::LineStrip, globalTransform);
    }

    window->display();
}
