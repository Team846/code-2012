CFLAGS := `$(CROSS_COMPILE)pkg-config --cflags opencv`
LDFLAGS := `$(CROSS_COMPILE)pkg-config --libs opencv`
CC = $(CROSS_COMPILE)g++
EXEFILES = 

all: $(EXEFILES) KeyTracker

KeyTracker: ../headers/messenger.h ../source/messenger.cpp ../global.h ../cvFunction.h ../robot_main.cpp
	$(CC) $(CFLAGS) -o $@ ../source/messenger.cpp ../robot_main.cpp $(LDFLAGS) -D BEAGLEBOARD

Test: ../headers/messenger.h ../source/messenger.cpp ../global.h ../cvFunction.h ../test_main.cpp
	$(CC) $(CFLAGS) -o $@ ../source/messenger.cpp ../test_main.cpp $(LDFLAGS)

$(EXEFILES): %:%.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<  

