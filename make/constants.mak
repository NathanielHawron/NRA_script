PROJECT = NRA_SCRIPT
# Target to compile for. This can be: ARCH, WINDOWS
TARGET ?= ARCH
# Compiler to be used. Also include the language and version, if required.
CXX := g++ -std=c++17 -D DEBUG
# Configuration to be used. This can be: DEBUG, COVERAGE, RELEASE
CONFIG := RELEASE
# List of libraries to include. This can include local files (-L) or system files (-l).
LIBRARY = 
# Name of file with entry point, and generated executable. This will look in the main folder for a file ending in .cpp.
MAIN ?= compiler_test
MAIN_DIR ?= ./main
# List of include directories. This` should include the include directory for this project, as well as any librarys' include directories. Make sure to prepend -I in front of the directories.
INCLUDE = -I ./include
# Command line arguments to be passed to the executable when run.
ARGS ?=


BIN_DIR ?= ./.bin
LIB_DIR ?= lib
LIB ?= nra_script

# List of source files to include in the library. If the project is not a library, this can be blank.
FILES = ./src/*/*.cpp
