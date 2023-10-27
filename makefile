
################################################################################
#                               LIBRARY PATHS                                  #
################################################################################

LIB_DIR = ../libs/

export SFML_DIR    = $(LIB_DIR)SFML-2.6.0/
export PUGIXML_DIR = $(LIB_DIR)pugixml-1.14/
export EARCUT_DIR  = $(LIB_DIR)earcut.hpp-2.2.4/

################################################################################
#                            BINARY AND ARCHIVES                               #
################################################################################

BIN_NAME = astarcities.exe

ARCHIVE_FILES = obj/CLIENT/client.a \
                obj/MAPDOWNLOADER/mapdownloader.a \
                obj/MAPPARSER/mapparser.a \
                obj/MAPRENDERER/maprenderer.a \
                obj/MAP/map.a \
                $(PUGIXML_DIR)build/make-g++-debug-standard-c++11/src/pugixml.cpp.o

################################################################################
#                                 TOOLS                                        #
################################################################################

LINKER = g++

MAKE = make

################################################################################
#                               COMPILER FLAGS                                 #
################################################################################

WARNING_FLAGS = -Wall \
                -Wextra \
                -pedantic \
                -Wnon-virtual-dtor \
                -Wold-style-cast \
                -Wcast-align \
                -Wunused \
                -Woverloaded-virtual \
                -Wpedantic \
                -Wconversion \
                -Wsign-conversion \
                -Wmisleading-indentation \
                -Wduplicated-cond \
                -Wduplicated-branches \
                -Wlogical-op \
                -Wnull-dereference \
                -Wuseless-cast \
                -Wdouble-promotion \
                -Wformat=2 \
                -Wimplicit-fallthrough

export GENERAL_COMPILER_FLAGS = -std=c++23 \
                                -MMD \
                                -fno-rtti \
                                $(WARNING_FLAGS)

ifeq ($(MAKECMDGOALS),)
    BUILD_DEBUG = 1
else ifeq ($(MAKECMDGOALS),debug)
    BUILD_DEBUG = 1
else ifeq ($(MAKECMDGOALS),release)
    BUILD_RELEASE = 1
endif

ifdef BUILD_DEBUG
    $(info Make debug)
    GENERAL_COMPILER_FLAGS += -g -Og
else ifdef BUILD_RELEASE
    $(info Make release)
    GENERAL_COMPILER_FLAGS += -O3
endif

################################################################################
#                               LINKER FLAGS                                   #
################################################################################

SFML_LINKER_FLAGS = -L$(SFML_DIR)lib \
                    -lsfml-network-s \
                    -lsfml-graphics-s \
                    -lsfml-window-s \
                    -lsfml-system-s \
                    -lwinmm \
                    -lopengl32 \
                    -lgdi32 \
                    -lws2_32 \
                    -lfreetype

LINKER_FLAGS = $(SFML_LINKER_FLAGS) \
               -static-libgcc \
               -static-libstdc++

################################################################################
#                               BUILD RULES                                    #
################################################################################

debug: all

release: all

all: $(BIN_NAME)

test : build_subprojects $(BIN_NAME)
	./$(BIN_NAME) maps/speyer.osm

clean: build_subprojects
	rm -rf $(BIN_NAME)

################################################################################
#                              BUILD BINARY                                    #
################################################################################

$(BIN_NAME) : $(ARCHIVE_FILES) build_subprojects
	@echo Link archives to executable '$@'
	@$(LINKER) $(ARCHIVE_FILES) -o $@ $(LINKER_FLAGS)

################################################################################
#                           BUILD SUBPROJECTS                                  #
################################################################################

BUILD_TARGET = all

ifeq ($(MAKECMDGOALS),clean)
    BUILD_TARGET = clean
endif

$(ARCHIVE_FILES) : build_subprojects

build_subprojects:
	@$(MAKE) -C src/MAP/           $(BUILD_TARGET)
	@$(MAKE) -C src/MAPPARSER/     $(BUILD_TARGET)
	@$(MAKE) -C src/MAPRENDERER/   $(BUILD_TARGET)
	@$(MAKE) -C src/CLIENT/        $(BUILD_TARGET)
