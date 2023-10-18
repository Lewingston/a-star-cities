
#include <iostream>

#include "MAPPARSER/mapparser.h"
#include "MAPRENDERER/maprenderer.h"

using namespace AStarCities;

int main(int argc, char** args) {

    if (argc != 2) {
        std::cout << "Pass path to .osm file as parameter" << std::endl;
        return 1;
    }

    const std::string osmFilePath = std::string(args[1]);

    MapRenderer renderer;
    renderer.openWindow();

    MapParser parser;
    parser.parseMap(parser.loadFromFile(osmFilePath));

    renderer.setMap(parser.getMap());
    renderer.runSimulation();

    return 0;

}
