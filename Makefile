CC = g++
CFlAGS = -g -I. -c -Wall
LFLAGS = -g -Wall

SRCS = main.cpp 
OBJS = main.o 
EXE = main
BUILD_DIR = ./build
LIBS = -ljsoncpp

all: $(OBJS)
	cd $(BUILD_DIR) && $(CC) $(LFLAGS) -o $(EXE) $(OBJS) $(LIBS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

main.o : main.cpp
	$(CC) $(CFlAGS) -o $(BUILD_DIR)/main.o main.cpp

run: all
	$(BUILD_DIR)/$(EXE)

clean:
	rm -rf $(BUILD_DIR)