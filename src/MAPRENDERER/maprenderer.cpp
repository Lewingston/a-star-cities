
#include "maprenderer.h"

#include "MAP/map.h"

#include "SOLVER/solver.h"

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

    //intersectionCircle.setFillColor(sf::Color(100, 250, 50));
    intersectionCircle.setFillColor(sf::Color(255, 255, 255));
}

void MapRenderer::openWindow(uint32_t width, uint32_t height) {

    //globalTransform.translate(sf::Vector2f(500, 150));

    resWidth = width;
    resHeight = height;

    window = std::shared_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(sf::Vector2(width, height)), "A* Cities"));
    window->setFramerateLimit(60);
    window->clear(sf::Color::White);
    window->display();
}

void MapRenderer::setMap(std::shared_ptr<Map> map) {

    this->map = map;

    for (const auto& pair : map->getRoads()) {
        const Road& road = pair.second;
        roads.insert({road.getId(), RoadRenderer(road, roadColorMap[road.getType()])});
    }

    for (const auto& pair : map->getBuildings()) {
        const Building& building = pair.second;
        buildings.push_back(BuildingRenderer(building, buildingColorMap[building.getType()]));
    }

    createBoundingBox(static_cast<float>(map->getLocalWidth()), static_cast<float>(map->getLocalHeight()));

    const float translateX = (static_cast<float>(map->getLocalWidth()) - static_cast<float>(resWidth)) / 2;
    const float translateY = (static_cast<float>(map->getLocalHeight()) - static_cast<float>(resHeight)) / 2;

    globalTransform.translate(sf::Vector2f(-translateX, -translateY));
}

void MapRenderer::setSolver(std::shared_ptr<Solver> solver) {
    this->solver = solver;
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

        doSolutionStep();
    }
}

void MapRenderer::doSolutionStep() {

    if (!solver)
        return;

    fadeRoads();

    if (solver->isDone())
        return;

    std::size_t doSteps = 1 + (solver->getOpenListSize() / 20);
    for (std::size_t ii = 0; ii < doSteps; ii++) {

        Road const* road = nullptr;
        while (road == nullptr && !solver->isDone()) {
            road = solver->doSubStep();
        }

        if (road != nullptr) {
            RoadRenderer& roadRenderer = roads.find(road->getId())->second;
            roadRenderer.setColor(255);
            whiteRoads.insert(roadRenderer);
        }
    }

    if (solver->isDone()) {
        for (const Road& road : solver->getSolution()) {
            RoadRenderer& roadRenderer = roads.find(road.getId())->second;
            roadRenderer.setColor(1'000'000);
            whiteRoads.insert(roadRenderer);
        }
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
            //doSolutionStep();
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
        for (const auto& [id, road] : roads) {
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

    if (showInterchanges) {
        drawInterchanges();
    }

    if (solver) {
        drawInterchange(solver->getStart());
        drawInterchange(solver->getEnd());
    }

    window->display();
}

void MapRenderer::drawInterchanges() {
    for (const auto& [id, intersection] : map->getIntersections()) {
        drawInterchange(intersection);
    }
}

void MapRenderer::drawInterchange(const Intersection& intersection) {
    auto [posX, posY] = intersection.getPosition();
    intersectionCircle.setPosition(sf::Vector2f(static_cast<float>(posX) - INTERSECTION_MARKER_SIZE, static_cast<float>(posY) - INTERSECTION_MARKER_SIZE));
    window->draw(intersectionCircle, globalTransform);
}

void MapRenderer::fadeRoads() {

    for (RoadRenderer& road : whiteRoads) {

        sf::Color targetColor = roadColorMap[road.getRoad().getType()];
        double color = road.getColor();

        if (solver->isDone())
            color = targetColor.r + (color - targetColor.r) * 0.99;
        else
            color = targetColor.r + (color - targetColor.r) * 0.998;

        if (color < targetColor.r) {
            whiteRoads.erase(road);
            road.setColor(targetColor);
        } else {
            road.setColor(color);
        }
    }
}
