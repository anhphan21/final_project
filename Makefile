CC=g++
CXXFLAGS = -std=c++98 -O2  
LDFLAGS = -lpthread
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:src/%.cpp=bin/%.o)
EXECUTABLE = bin/fp
INCLUDES = src/Bin.h src/Node.h src/Module.h src/Net.h src/Pin.h src/Placement.h src/Rectangle.h src/Row.h src/CellLibrary.h src/Database.h src/DatabaseDef.h 

# # Debug flags
# DEBUG_FLAGS = -g -O0

# # To enable debug mode, run: make DEBUG=1
# ifdef DEBUG
#     CXXFLAGS += $(DEBUG_FLAGS)	
# endif

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

bin/%.o: src/%.cpp $(INCLUDES)
	@mkdir -p bin
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf bin/*.o $(EXECUTABLE)