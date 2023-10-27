
#include <iostream>

#include "MAPPARSER/mapparser.h"
#include "MAPRENDERER/maprenderer.h"

using namespace AStarCities;

void richMap(const std::string& filePath);
void pathMap(const std::string& filePath);

int main(int argc, char** args) {

    if (argc != 2) {
        std::cout << "Pass path to .osm file as parameter" << std::endl;
        return 1;
    }

    const std::string osmFilePath = std::string(args[1]);

    richMap(osmFilePath);
    //pathMap(osmFilePath);

    return 0;

}

void richMap(const std::string& filePath) {

    MapRenderer renderer;
    renderer.setBackgroundColor(sf::Color(255, 255, 255));

    for (RoadType type : RoadType::getAll()) {
        renderer.setRoadColor(type, sf::Color(255, 0, 255));
    }

    renderer.setRoadColor(RoadType::MOTORWAY,       sf::Color(0, 0, 0));
    renderer.setRoadColor(RoadType::MOTORWAY_LINK,  sf::Color(0, 0, 0));
    renderer.setRoadColor(RoadType::TRUNK,          sf::Color(0, 0, 0));
    renderer.setRoadColor(RoadType::TRUNK_LINK,     sf::Color(0, 0, 0));
    renderer.setRoadColor(RoadType::PRIMARY,        sf::Color(0, 0, 0));
    renderer.setRoadColor(RoadType::PRIMARY_LINK,   sf::Color(0, 0, 0));

    renderer.setRoadColor(RoadType::SECONDARY,      sf::Color(0, 0, 0));
    renderer.setRoadColor(RoadType::SECONDARY_LINK, sf::Color(0, 0, 0));
    renderer.setRoadColor(RoadType::TERTIARY,       sf::Color(0, 0, 0));
    renderer.setRoadColor(RoadType::TERTIARY_LINK,  sf::Color(0, 0, 0));
    renderer.setRoadColor(RoadType::UNCLASSIFIED,   sf::Color(0, 0, 0));
    renderer.setRoadColor(RoadType::RESIDENTIAL,    sf::Color(0, 0, 0));
    renderer.setRoadColor(RoadType::LIVING_STREET,  sf::Color(80, 80, 80));

    renderer.setRoadColor(RoadType::SERVICE,        sf::Color(160, 160, 160));
    renderer.setRoadColor(RoadType::CONSTRUCTION,   sf::Color(120, 120, 120));

    renderer.setRoadColor(RoadType::PEDESTRIAN,     sf::Color(175, 175, 175));
    renderer.setRoadColor(RoadType::CYCLEWAY,       sf::Color(220, 220, 220));
    renderer.setRoadColor(RoadType::FOOTWAY,        sf::Color(220, 220, 220));
    renderer.setRoadColor(RoadType::PATH,           sf::Color(220, 220, 220));
    renderer.setRoadColor(RoadType::STEPS,          sf::Color(220, 220, 220));
    renderer.setRoadColor(RoadType::TRACK,          sf::Color(220, 220, 220));
    renderer.setRoadColor(RoadType::RAZED,          sf::Color(220, 220, 220));
    renderer.setRoadColor(RoadType::CORRIDOR,       sf::Color(220, 220, 220));
    renderer.setRoadColor(RoadType::PLATFORM,       sf::Color(100, 175, 195));
    renderer.setRoadColor(RoadType::BRIDLEWAY,      sf::Color(0, 160, 0));

    for (BuildingType type : BuildingType::getAll()) {
        renderer.setBuildingColor(type, sf::Color(255, 0, 255));
    }

    renderer.setBuildingColor(BuildingType::APPARTMENTS,        sf::Color(0, 125, 0));
    renderer.setBuildingColor(BuildingType::RESIDENTIAL,        sf::Color(0, 125, 0));
    renderer.setBuildingColor(BuildingType::SEMIDETACHED_HOUSE, sf::Color(40, 205, 45));
    renderer.setBuildingColor(BuildingType::DETACHED,           sf::Color(40, 205, 45));
    renderer.setBuildingColor(BuildingType::HOUSE,              sf::Color(40, 205, 45));
    renderer.setBuildingColor(BuildingType::BUNGALOW,           sf::Color(40, 205, 45));

    renderer.setBuildingColor(BuildingType::DORMITORY,          sf::Color(159, 90, 196));
    renderer.setBuildingColor(BuildingType::HOTEL,              sf::Color(152, 62, 201));

    renderer.setBuildingColor(BuildingType::COMMERCIAL,         sf::Color(8, 71, 196));
    renderer.setBuildingColor(BuildingType::SUPERMARKET,        sf::Color(8, 71, 196));
    renderer.setBuildingColor(BuildingType::RETAIL,             sf::Color(75, 140, 225));
    renderer.setBuildingColor(BuildingType::OFFICE,             sf::Color(82, 232, 235));
    renderer.setBuildingColor(BuildingType::INDUSTRIAL,         sf::Color(201, 150, 8));
    renderer.setBuildingColor(BuildingType::WAREHOUSE,          sf::Color(201, 164, 62));

    renderer.setBuildingColor(BuildingType::RELIGIOUS,          sf::Color(97, 49, 2));

    renderer.setBuildingColor(BuildingType::FIRE_STATION,       sf::Color(210, 0, 0));
    renderer.setBuildingColor(BuildingType::HOSPITAL,           sf::Color(210, 0, 0));
    renderer.setBuildingColor(BuildingType::TRAIN_STATION,      sf::Color(125, 125, 125));
    renderer.setBuildingColor(BuildingType::TRANSPORTATION,     sf::Color(125, 125, 125));
    renderer.setBuildingColor(BuildingType::KINDERGARTEN,       sf::Color(177, 117, 255));
    renderer.setBuildingColor(BuildingType::SCHOOL,             sf::Color(161, 89, 255));
    renderer.setBuildingColor(BuildingType::COLLEGE,            sf::Color(138, 48, 255));
    renderer.setBuildingColor(BuildingType::UNIVERSITY,         sf::Color(117, 10, 255));
    renderer.setBuildingColor(BuildingType::MUSEUM,             sf::Color(117, 10, 255));

    renderer.setBuildingColor(BuildingType::CIVIC,              sf::Color(255, 61, 177));
    renderer.setBuildingColor(BuildingType::PUBLIC,             sf::Color(255, 61, 177));
    renderer.setBuildingColor(BuildingType::GOVERNMENT,         sf::Color(255, 61, 177));

    renderer.setBuildingColor(BuildingType::YES, sf::Color(128, 128, 128));

    renderer.openWindow();

    MapParser parser;
    parser.parseRoadTypes(RoadType::getAll());
    parser.parseMap(parser.loadFromFile(filePath));

    renderer.setMap(parser.getMap());
    renderer.runSimulation();
}

void pathMap(const std::string& filePath) {

    MapRenderer renderer;

    renderer.openWindow();

    std::set<RoadType> roadTypes = RoadType::ROADS;
    roadTypes.insert(RoadType::LINKS.begin(), RoadType::LINKS.end());
    roadTypes.insert(RoadType::LIVING_STREET);

    MapParser parser;
    parser.parseRoadTypes(roadTypes);
    parser.parseBuildings(false);
    parser.parseMap(parser.loadFromFile(filePath));

    renderer.setMap(parser.getMap());
    renderer.runSimulation();
}
