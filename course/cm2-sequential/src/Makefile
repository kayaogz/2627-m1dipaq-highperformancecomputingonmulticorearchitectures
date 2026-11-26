# Sessions 2 and 3 - Sequential code optimization: the example programs of the lecture
#
#   make                    build the examples into build/
#   make run                build and run them all
#   make run-01-dot-sums    build and run one of them (the numbers follow the
#                           lecture notes and the slides)
#   make clean              remove build/
#
# These programs are written for the MyDocker server of the course (g++ 16,
# options -O2 -fno-tree-vectorize). They need an Intel or AMD processor and
# GNU g++: they read the name of the processor and measure its frequency with
# x86 instructions, so they do not compile for another processor (an Apple
# M-series one, for example).
#
# The compiler is the most recent GNU g++ found on the machine; override with
# make CXX=g++-16  if needed. (":=" rather than "?=", because make has a
# built-in default for CXX that "?=" would not override.)

CXX      := $(shell command -v g++-16 || command -v g++-15 || command -v g++-14 || echo g++)
CXXFLAGS ?= -O2 -fno-tree-vectorize -Wall
BUILD    := build

# numbered in the order of the lecture notes
PROGRAMS := 01-dot-sums 02-filter 03-obstacles

all: $(addprefix $(BUILD)/,$(PROGRAMS))

$(BUILD)/%: %.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

run: $(addprefix run-,$(PROGRAMS))

run-%: $(BUILD)/%
	@echo "=== $* ==="
	@./$<
	@echo

clean:
	rm -rf $(BUILD)

.PHONY: all run clean
