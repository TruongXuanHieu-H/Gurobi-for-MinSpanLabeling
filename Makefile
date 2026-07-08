GUROBI_HOME=/opt/gurobi1302/linux64

CXX=g++

CXXFLAGS= \
    -std=c++20 \
    -O3 \
    -fPIC \
    -fexceptions \
    -DNDEBUG \
	-I$(GUROBI_HOME)/include \
    -Isrc

LDFLAGS= \
    -L$(GUROBI_HOME)/lib

LIBS= \
    -lgurobi_c++ \
	-lgurobi130 \
	-lm \
	-lpthread \
	-ldl

TARGET := build/gurobi

SRC := $(shell find src -type f -name '*.cpp')
OBJ := $(patsubst src/%.cpp,build/obj/%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p $(dir $@)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)

build/obj/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build

run: $(TARGET)
	./$(TARGET)