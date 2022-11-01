############################################################
# Variables for the Makefile

CXX=g++
CXXVER=--std=c++11
SOURCE=.
BUILD=.
INCLUDE=.
CXXFLAGS=-I$(INCLUDE) -g $(CXXVER)
OUTPUT=$(BUILD)/pluck

ifeq ($(OS),Windows_NT)
OUTPUT:=$(OUTPUT).exe
endif

############################################################
# Macros for all projects

.PHONY: all
all: build testall

.PHONY: clean
clean:
	@echo Cleaning all projects...
	rm -rf $(OUTPUT) output.wav *.stackdump

############################################################
# Building program

.PHONY: build
build: $(OUTPUT)

$(OUTPUT): $(SOURCE)/pluck.cpp
	@echo Building Project 6 \($@\)...
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: testall
testall:
	$(OUTPUT)

############################################################
# Testing program

.PHONY: run
run: build
	@echo -------------------------------------
	$(OUTPUT)
	@echo -------------------------------------
