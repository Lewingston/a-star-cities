
C_FILES = main.cpp

SRC_DIR = ./

OBJ_DIR = ../../obj/CLIENT/

ARCHIVE_NAME = client

CFLAGS = $(GENERAL_COMPILER_FLAGS)

all : $(OBJ_DIR)$(ARCHIVE_NAME).a

clean:
	rm -rf $(OBJ_DIR)

INCLUDE_FLAGS =

INCLUDE_FLAGS = -I../ \
                -isystem../../$(SFML_DIR)include

include ../comCppMak.mak
