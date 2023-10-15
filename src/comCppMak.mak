################################################################################
#       Common makefile to compile multiple .cpp flies to single .a file       #
################################################################################
#
# The following variables have to be set for this makefile to work properly
#
#    C_FILES        list of all source files to be compiled
#
#    SRC_DIR        source directory where the cfiles are located
#
#    OBJ_DIR        output directory for the generated o files
#
#    ARCHIVE_NAME   name of the generated .a file
#
# Addtional optional variabels
#
#    INCLUDE_FLAGS  flags for addional include directories
#
#    CFLAGS         additional build flags
#
# The including makefile must invoke the rule for $(OBJ_DIR)$(ARCHIVE_NAME).a
# for all rules to be applied

################################################################################
#                              PERFROM CHECKS                                  #
################################################################################

ifndef C_FILES
    $(error "C_FILES" ist not defined!)
endif

ifndef SRC_DIR
    $(error "SRC_DIR ist not defined!")
endif

ifndef OBJ_DIR
    $(error "OBJ_DIR ist not defined!")
endif

ifndef ARCHIVE_NAME
    $(error "ARCHIVE_NAME ist not defined!")
endif

################################################################################
#                              COMPILER SECTION                                #
################################################################################

# Compiler to generate .o files from .cpp files
COM = g++

# Archiver to generate .a file from .o files
AR = ar

################################################################################
#                              COMPILER FLAGS                                  #
################################################################################

ifndef INCLUDE_FLAGS
    INCLUDE_FLAGS =
endif

ifndef CFLAGS
    CFLAGS =
endif

################################################################################
#                            FILE AND DIRECTORY SECTION                        #
################################################################################

# list of object files
O_FILES = $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(C_FILES))))

# list of dependeny files
D_FILES = $(addprefix $(OBJ_DIR),$(addsuffix .d,$(basename $(C_FILES))))

# list of output directories
OUT_DIRS = $(abspath $(addprefix $(OBJ_DIR),$(sort $(dir $(C_FILES)))))

################################################################################
#                         INCLUDE DEPENDENCY FILES                             #
################################################################################

-include $(D_FILES)

################################################################################
#                       RULE TO GENERATE ARCHIVE FILE                          #
################################################################################

$(OBJ_DIR)$(ARCHIVE_NAME).a : $(O_FILES) | $(OUT_DIRS)
	@echo Create archive $@
	@$(AR) -rc $@ $(O_FILES)

################################################################################
#                  RULE TO COMPILE SOURCE FILE TO OBJECT FILE                  #
################################################################################

define generic_cpp_to_o_rule

$(OBJ_DIR)$(basename $(Item)).o : $(Item) | $(abspath $(OBJ_DIR)$(dir $(Item)))
	@echo Compile $(Item)
	@$(COM) $(CFLAGS) $(INCLUDE_FLAGS) -c $$< -o $$@

endef

$(foreach Item,$(C_FILES),$(eval $(generic_cpp_to_o_rule)))

################################################################################
#                      RULE TO GENERATE OUTPUT DIRECTORIES                     #
################################################################################

$(OUT_DIRS) :
	mkdir -p $@
