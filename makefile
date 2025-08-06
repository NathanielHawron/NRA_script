MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

include $(MAKEFILE_DIR)/make/colors.mak
include $(MAKEFILE_DIR)/make/constants.mak

override FILES := version
override FILES_HEADER_ONLY := 
override SRC := $(addprefix src/, $(addsuffix .cpp, $(FILES)))
override OBJ := $(addprefix .o/, $(addsuffix .o, $(FILES)))
override COV := $(addprefix coverage_file_, $(FILES))
override COV_LIB := $(addprefix coverage/, $(addsuffix .cpp.gcov, $(FILES))) $(addprefix coverage/, $(addsuffix .hpp.gcov, $(FILES))) $(addprefix coverage/, $(addsuffix .hpp.gcov, $(FILES_HEADER_ONLY)))

override MAIN_PATH := $(MAIN_DIR)/$(MAIN).cpp
override COVERAGE_PATH := $(MAIN_DIR)/$(MAIN)-$(MAIN)
override COVERAGE_O := $(BIN_DIR)/$(MAIN_DIR)/$(MAIN)_$(CONFIG)_$(TARGET)-$(MAIN)

FLAGS :=
ifeq ($(CONFIG), COVERAGE)
	FLAGS += -O0 -g --coverage
else ifeq ($(CONFIG), DEBUG)
	FLAGS += -O0 -g -D DEBUG
else
	CONFIG := RELEASE
endif

ifeq ($(TARGET), WIN)
	override TARGET := WINDOWS
endif

ifeq ($(TARGET), ARCH)
else ifeq ($(TARGET), WINDOWS)
else
	override TARGET := ARCH
endif

override BIN_PATH := $(BIN_DIR)/$(MAIN_DIR)/$(MAIN)_$(CONFIG)_$(TARGET)
override LIB_PATH := $(LIB_DIR)/$(LIB)_$(CONFIG)_$(TARGET).a



init:
	-mkdir -p .bin
	-mkdir -p .o
	-mkdir -p lib

.o/%.o: src/%.cpp
	$(CXX) $(FLAGS) -c $< -o $@ -I ./include

build: $(OBJ)
	ar rcs $(LIB_PATH) $(OBJ)

build_main: build
	@mkdir -p $(dir $(BIN_PATH))
	$(CXX) $(FLAGS) $(MAIN_PATH) $(LIB_PATH) -o $(BIN_PATH) -I ./include

run: build_main
	@echo -e "${GREEN}Running ${MAIN} ${NOCOLOR}"
	$(BIN_PATH)
	@echo -e "${GREEN}Finished running ${MAIN} ${NOCOLOR}"

coverage_clear:
	@mkdir coverage -p
	-@rm ./coverage/* -r
	@mkdir coverage/lib
	@mkdir coverage/std
	-@rm ./*.gcno
	-@rm ./*.gcda
	-@rm ./*.gcov

coverage_file_%:
	gcov -o .o src/$*
coverage: clear coverage_clear run $(COV)
	gcov -o $(COVERAGE_O) $(COVERAGE_PATH)
	@mv  ./*.gcov ./coverage
	@for f in $(COV_LIB); do \
		[ -e "$$f" ] && mv "$$f" ./coverage/lib/; \
	done
	@mv coverage/*.gcov coverage/std/
#	@echo $(COV_LIB)

clear:
	-@rm $(BIN_DIR)/* -r
	-@rm .o/* -r
	-@mkdir -p .o/curve
	-@rm $(LIB_DIR)/*
