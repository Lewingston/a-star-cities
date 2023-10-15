
C_FILES = maprenderer.cpp \
          roadrenderer.cpp \
          buildingrenderer.cpp

SRC_DIR = ./

OBJ_DIR = ../../obj/MAPRENDERER/

ARCHIVE_NAME = maprenderer

CFLAGS = $(GENERAL_COMPILER_FLAGS)

all : $(OBJ_DIR)$(ARCHIVE_NAME).a

clean:
	rm -rf $(OBJ_DIR)

INCLUDE_FLAGS =

INCLUDE_FLAGS = -I../ \
                -isystem../../../libs/SFML-2.6.0/include \
                -DSFML_STATIC

include ../comCppMak.mak
