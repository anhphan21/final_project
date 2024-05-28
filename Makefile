CC=g++
LDFLAGS=-std=c++11 -O3 -lm
SOURCES= src/Placement.cpp src/main.cpp src/Database.cpp 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=fp
INCLUDES=src/Bin.h src/Module.h src/Net.h src/Pin.h src/Placement.h src/Rectangle.h src/Row.h src/CellLibrary.h src/Database.h src/DatabaseDef.h 
all: $(SOURCES) bin/$(EXECUTABLE)

bin/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o:  %.c  ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o bin/$(EXECUTABLE)
