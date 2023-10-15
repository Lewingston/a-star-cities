
#include "maprenderer.h"

#include "MAP/map.h"

#include "SFML/Graphics.hpp"

#include <iostream>

using namespace AStarCities;

void MapRenderer::openWindow() {

    window = std::shared_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(sf::Vector2(1400u, 1400u)), "A* Cities"));
    window->setFramerateLimit(60);
    window->clear(sf::Color::White);
    window->display();

}

void MapRenderer::setMap(std::shared_ptr<Map> map) {

    this->map = map;

    for (const auto& pair : map->getRoads()) {
        const Road& road = pair.second;
        roads.push_back(RoadRenderer(road));
    }
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
    if (event.key.code == sf::Keyboard::Enter) {

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

    window->clear(sf::Color::White);

    sf::RectangleShape rect(sf::Vector2f(static_cast<float>(map->getLocalWidth()), static_cast<float>(map->getLocalHeight())));
    rect.setPosition(sf::Vector2f(200, 200));
    rect.setOutlineThickness(1);
    rect.setOutlineColor(sf::Color::Red);
    rect.setFillColor(sf::Color::White);
    window->draw(rect, globalTransform);

    for (RoadRenderer& road : roads) {
        road.draw(window, globalTransform);
    }

    window->display();
}
