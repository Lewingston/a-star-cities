# a-star-cities

Visualisation of  A* path finding algorithm usign open street map data.

The map data can be downloaded via the [open street map API](https://wiki.openstreetmap.org/wiki/Downloading_data).
The data must then be saved as .osm file to be used by the program.

Inspired by this [video](https://www.youtube.com/watch?v=CgW0HPHqFE8).

## Usage

Pass the osm map data as program parameter.

```
astarcities.exe mapdata.osm
```

## Demo

![Demo](docs/astar_demo.gif)

## Known Issues

Large mpas leads to very bad performance.

Very largs maps might lead to a crash of the program due to not enough stack memory.

## Dependencies

[SFML 2.6.0](https://github.com/SFML/SFML/releases/tag/2.6.0)<br>
[pugixml v1.14](https://github.com/zeux/pugixml/releases/tag/v1.14)<br>
[mapbox/earcut.hpp v2.2.4](https://github.com/mapbox/earcut.hpp/releases/tag/v2.2.4)
