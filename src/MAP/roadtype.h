#pragma once

#include <cstdint>
#include <string>
#include <map>

namespace AStarCities {

    class RoadType {

        public:

            enum Type : uint32_t {
                UNKNOWN = 0,
                // ===== ROADS =====
                MOTORWAY,
                TRUNK,
                PRIMARY,
                SECONDARY,
                TERTIARY,
                UNCLASSIFIED,
                RESIDENTIAL,
                // ===== LINK ROADS =====
                MOTORWAY_LINK,
                TRUNK_LINK,
                PRIMARY_LINK,
                SECONDARY_LINK,
                TERTIARY_LINK,
                // ===== SPECIAL ROAD TYPES =====
                LIVING_STREET, // Spielstraße
                SERVICE,
                PEDESTRIAN,
                TRACK,
                BUS_GUIDEWAY,
                ESCAPE,
                ROAD,
                BUSWAY,
                // ===== PATHS ======
                FOOTWAY,
                BRIDLEWAY, // Reitweg
                STEPS,
                CORRIDOR, // Passage
                PATH,
                VIA_FERRATA, // Klettersteig
                // ===== CYCLEWAY =====
                CYCLEWAY,
                // ===== LIFECYCLE =====
                CONSTRUCTION,
                PROPOSED,
                // ===== OTHER HIGHWAY FEATURES =====
                ELEVATOR,
                EMERGENCY_BAY,
                PLATFORM,
                REST_AREA,
                SERVICES,
                BUS_STOP,
                CROSSING,
                RAZED,
            };

            [[nodiscard]] std::string toString() const;

            RoadType() = default;

            RoadType(const std::string& type);

            RoadType(Type type) : type(type) {}

            virtual ~RoadType() = default;

            bool operator==(Type type)     const { return type ==  this->type; }
            bool operator==(RoadType type) const { return this->type == type.type; }
            bool operator<(RoadType type)  const { return this->type < type.type; }

            [[nodiscard]] Type getEnumValue() const { return type; }


        private:

            Type type = Type::UNKNOWN;

            static const std::map<std::string, Type> typeNameMap;
    };
}
