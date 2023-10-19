
#include "roadtype.h"

using namespace AStarCities;

const std::map<std::string, RoadType::Type> RoadType::typeNameMap = {
    // ===== ROADS =====
    { "motorway",       MOTORWAY       },
    { "trunk",          TRUNK          },
    { "primary",        PRIMARY        },
    { "secondary",      SECONDARY      },
    { "tertiary",       TERTIARY       },
    { "unclassified",   UNCLASSIFIED   },
    { "residential",    RESIDENTIAL    },
    // ==== LINK ROADS =====
    { "motorway_link",  MOTORWAY_LINK  },
    { "trunk_link",     TRUNK_LINK     },
    { "primary_link",   PRIMARY_LINK   },
    { "secondary_link", SECONDARY_LINK },
    { "tertiary_link",  TERTIARY_LINK  },
    // ===== SPECIAL ROAD TYPES =====
    { "living_street",  LIVING_STREET  },
    { "service",        SERVICE        },
    { "pedestrian",     PEDESTRIAN     },
    { "track",          TRACK          },
    { "bus_guideway",   BUS_GUIDEWAY   },
    { "escape",         ESCAPE         },
    { "road",           ROAD           },
    { "busway",         BUSWAY         },
    // ====== PATHS =====
    { "footway",        FOOTWAY        },
    { "bridleway",      BRIDLEWAY      },
    { "steps",          STEPS          },
    { "corridor",       CORRIDOR       },
    { "path",           PATH           },
    { "via_ferrata",    VIA_FERRATA    },
    // ===== CYCLEWAY =====
    { "cycleway",       CYCLEWAY       },
    // ===== LIFECYCLE =====
    { "construction",   CYCLEWAY       },
    // ===== OTHER HIGHWAY FEATURES =====
    { "elevator",       ELEVATOR       },
    { "emergancy_bay",  EMERGENCY_BAY  },
    { "platform",       PLATFORM       },
    { "rest_area",      REST_AREA      },
    { "services",       SERVICES       },
    { "bus_stop",       BUS_STOP       },
    { "crossing",       CROSSING       },
    { "proposed",       PROPOSED       },
    { "razed",          RAZED          }
};

RoadType::RoadType(const std::string& type) {
    if (auto find = typeNameMap.find(type); find != typeNameMap.end()) {
        this->type = find->second;
    } else {
        this->type = UNKNOWN;
    }
}
