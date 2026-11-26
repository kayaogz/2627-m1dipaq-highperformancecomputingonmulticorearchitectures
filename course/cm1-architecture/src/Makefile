# Session 1 - The anatomy of a CPU core architecture: the example program of the session
#
#   make               build the example into build/
#   make run           build and run it
#   make run-01-dot    the same (the numbers follow the lecture notes and the slides)
#   make clean         remove build/
#
# The compiler is the most recent GNU g++ found on the machine (on macOS, the
# Homebrew one); override with  make CXX=g++-16  if needed. (":=" rather than
# "?=", because make has a built-in default for CXX that "?=" would not override.)

CXX      := $(shell command -v g++-16 || command -v g++-15 || command -v g++-14 || echo g++)
CXXFLAGS ?= -O2 -Wall
BUILD    := build

# numbered in the order of the lecture notes
PROGRAMS := 01-dot

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
