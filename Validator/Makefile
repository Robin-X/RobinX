APP = RobinX
DEBUG =  -g -fstack-protector #-Wall -Wvla -Werror -D_GLIBCXX_DEBUG
RELEASE = -Ofast -frename-registers -funroll-loops -march=native

ROBINX = ./source/include

CC = g++
LD = g++

CFLAGS += -std=c++11 -I${ROBINX}
LDFLAGS += -lpthread -ldl -I${ROBINX}

# Use all threads to compile in parallel
MAKEFLAGS += -j 8

MODULES   := $(shell ls ./source/)
SRC_DIR   := $(addprefix source/,$(MODULES))
BUILD_DIR := $(addprefix build/,$(MODULES))

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst source/%.cpp,build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR))

vpath %.cpp $(SRC_DIR)

define make-goal
$1/%.o: %.cpp
	@echo "[CC] $<"
	$(CC) $(CFLAGS) $(DEBUG) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs build/$(APP)
build/$(APP): $(OBJ)
	@echo "[LD] $<"
	$(LD) $(ALLPADBis)  $^ $(LDFLAGS) -o $(APP)

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)
	@rm $(APP)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
