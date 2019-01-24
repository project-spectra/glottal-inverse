##--- Copyright notice

# MIT License
# 
# Copyright (c) 2019 Chloé Ichinose
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

##--- Constants and configuration

TARGET := gif

# Modify as necessary
CXXFLAGS += #-ftree-vectorize 
LDFLAGS += -lgsl -lcblas -lm -lportaudio

SRC_DIR := src
INC_DIR := inc
BIN_DIR := bin

# Hidden directories for intermediate files
OBJ_DIR := .obj
DEP_DIR := .dep

ifndef BUILD_TYPE
	BUILD_TYPE := Debug
endif

##--- Toolchain flags

CXXFLAGS += -Wall -Wextra -Wno-sign-compare -Werror=implicit-function-declaration -pipe -fPIC -fstack-protector-strong

CXXFLAGS_Debug := -g -O2
LDFLAGS_Debug := 

CXXFLAGS_Release := -O2 -Os -Ofast
LDFLAGS_Release :=

CXXFLAGS += $(CXXFLAGS_$(BUILD_TYPE))
LDFLAGS += $(LDFLAGS_$(BUILD_TYPE))

CXXFLAGS += -I$(INC_DIR)

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.Td

##--- Functions

# Compile source file $1 to object file $2
define compile
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) -c $1 -o $2
	@mv -f $(DEP_DIR)/$*.Td $(DEP_DIR)/$*.d
	@touch $@
endef

# Link object files $1 to binary $2
define link
	$(CXX) $1 -o $2 $(LDFLAGS)
endef

# Mirror directory structure from $1 to $2
define cpdirtree
	find "$1" -type d | sed -e "s?$1?$2?" | xargs mkdir -p
endef

##--- Init stuff

# Include directory is optional to build.
# Binary directory will be created as needed.
$(shell mkdir -p $(SRC_DIR) $(OBJ_DIR) $(DEP_DIR) >/dev/null)

# Mirror source directory in object and dependency directories.
$(shell $(call cpdirtree,$(SRC_DIR),$(OBJ_DIR)))
$(shell $(call cpdirtree,$(SRC_DIR),$(DEP_DIR)))

# List source file
CXX_FILES := $(shell find $(SRC_DIR) -type f -iname "*.cpp")
H_FILES := $(shell find $(INC_DIR) -type f -iname "*.h" 2>/dev/null)

# Map each source file to an object and a dependency file.
O_FILES := $(CXX_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
D_FILES := $(CXX_FILES:$(SRC_DIR)/%.cpp=$(DEP_DIR)/%.d)

##--- Phony make rules

.PHONY: $(TARGET) all clean re

all: $(TARGET)

$(TARGET): $(BIN_DIR)/$(TARGET)

clean:
	-rm -f $(O_FILES) $(D_FILES)

clean-dist: clean
	-rm -f $(BIN_DIR)/$(TARGET)

re: clean-dist all

##--- Building make rules

$(BIN_DIR)/$(TARGET): $(O_FILES)
	@mkdir -p $(BIN_DIR)
	$(call link,$+,$@)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEP_DIR)/%.d
	$(call compile,$<,$@)

$(DEP_DIR)/%.d: ;
.PRECIOUS: $(DEP_DIR)/%.d

-include $(D_FILES)

