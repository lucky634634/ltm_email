CC = g++
CFlAGS = -g -I. -c -Wall
LFLAGS = -g -Wall

SRCS = main.cpp Config.cpp MailClient.cpp
OBJS = main.o Config.o MailClient.o
EXE = main
BUILD_DIR = ./build
LIBS = -ljsoncpp

all: $(OBJS)
	cd $(BUILD_DIR) && $(CC) $(LFLAGS) -o $(EXE) $(OBJS) $(LIBS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

main.o : $(BUILD_DIR) main.cpp
	$(CC) $(CFlAGS) -o $(BUILD_DIR)/main.o main.cpp

Config.o: $(BUILD_DIR) Config.cpp
	$(CC) $(CFlAGS) -o $(BUILD_DIR)/Config.o Config.cpp

MailClient.o: $(BUILD_DIR) MailClient.cpp
	$(CC) $(CFlAGS) -o $(BUILD_DIR)/MailClient.o MailClient.cpp
run: all
	$(BUILD_DIR)/$(EXE)

clean:
	rm -rf $(BUILD_DIR)