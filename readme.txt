Files:
	/src/main.cpp
	/src/Bin.h
	/src/CellLibrary.h
	/src/Database.cpp
	/src/Database.h
	/src/DatabaseDef.cpp
	/src/History.h
	/src/Module.h
	/src/Net.h
	/src/Node.h
	/src/Pin.h
	/src/Placement.h
	/src/Placement.cpp
	/src/Point.h
	/src/Rectangle.h
	/src/rhombus.h
	/src/Row.h
	/src/Timing.h
	/bin/fp
	makefile
	readme.txt

compile:
	make
	The fp file will be generated in /bin
run:
	./bin/fm <input_file_name> <output_file_name>