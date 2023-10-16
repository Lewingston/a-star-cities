
C_FILES = mapparser.cpp

SRC_DIR = ./

OBJ_DIR = ../../obj/MAPPARSER/

ARCHIVE_NAME = mapparser

CFLAGS = $(GENERAL_COMPILER_FLAGS)

all : $(OBJ_DIR)$(ARCHIVE_NAME).a

clean:
	rm -rf $(OBJ_DIR)

INCLUDE_FLAGS =

INCLUDE_FLAGS = -I../ \
                -I../../$(PUGIXML_DIR)src/

include ../comCppMak.mak
