
C_FILES = map.cpp \
          road.cpp \
          building.cpp \
          roadtype.cpp \
          buildingtype.cpp

SRC_DIR = ./

OBJ_DIR = ../../obj/MAP/

ARCHIVE_NAME = map

CFLAGS = $(GENERAL_COMPILER_FLAGS)

all : $(OBJ_DIR)$(ARCHIVE_NAME).a

clean:
	rm -rf $(OBJ_DIR)

INCLUDE_FLAGS =

INCLUDE_FLAGS = -I../

include ../comCppMak.mak
