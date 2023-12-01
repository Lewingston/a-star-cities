
C_FILES = solver.cpp

SRC_DIR = ./

OBJ_DIR = ../../obj/SOLVER/

ARCHIVE_NAME = solver

CFLAGS = $(GENERAL_COMPILER_FLAGS)

all : $(OBJ_DIR)$(ARCHIVE_NAME).a

clean:
	rm -rf $(OBJ_DIR)

INCLUDE_FLAGS = -I../ \
                -DSFML_STATIC

include ../comCppMak.mak
